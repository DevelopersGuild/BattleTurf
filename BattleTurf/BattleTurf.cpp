#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <string>
#include <stdio.h>
#include <iostream>
#include "box.h"

/*
Settings of the game. Well, I will change them into dynamic allocated after we make the menu.
*/
const int NUM_WALL = 6;         //number of wall
const int NUM_PLAYER = 4;       //number of player, must be less or equals to than 4
const int NUM_BOX_WIDTH = 6;    //the width of the map
const int NUM_BOX_HEIGHT = 6;   //the height of the map
const int NUM_SCORE_EACH_PLAYER = ((NUM_BOX_WIDTH * NUM_BOX_HEIGHT) - NUM_WALL) / NUM_PLAYER;       //the number of score box that each player can put in the game
const int BOX_SIZE = 55;        //the size of the box

void Show_Cursor_CurrentBox(Box *currentbox, Box *lastbox);
void Set_Wall(Box box[][NUM_BOX_HEIGHT], int numofwall);
void Set_Score_Pool(int player_score_order[][NUM_SCORE_EACH_PLAYER], int active_player);
void Check_adjacent(sf::Vector2i mouseposition,Box *currentbox, Box box[][NUM_BOX_HEIGHT],sf::Text score[][NUM_BOX_HEIGHT]);

int main()
{
/*******************************************************************
    Declare the important values of the game.
********************************************************************/
    srand(time(NULL));                              //randomize for rand() function.
    sf::Vector2i mouseposition;                     //a vector variable for the cursor's position
    sf::Clock clock;                                //to be used

    sf::Font font;                                  //the font of the score
    font.loadFromFile("arial.ttf");                 //...the font is arial.
    sf::Text score[NUM_BOX_WIDTH][NUM_BOX_HEIGHT];  //score[][]is the score for graphing only, it doesn't change the game.
    for(int i = 0; i < NUM_BOX_WIDTH; i++)          //initializing each element of score[][]
    {
        for(int j = 0; j < NUM_BOX_HEIGHT; j++)
        {
            score[i][j].setFont(font);
            score[i][j].setColor(sf::Color::Black);
            score[i][j].setCharacterSize(30);
            score[i][j].setStyle(sf::Text::Regular);
            score[i][j].setString("0");
            score[i][j].setPosition(BOX_SIZE * i, BOX_SIZE * j);
        }
    }

/*
Declare the variable of turn and turn_count.
active_player:  determine who can moves.
                0 = player1
                1 = player2
                2 = player3
                3 = player4

turn_count:     how many turn has passed.
*/
    int active_player = 0;
    int turn_count = 0;
/*
Declare the boxes and generate the walls.
*/
    Box box[NUM_BOX_WIDTH][NUM_BOX_HEIGHT];
    Set_Wall(box, NUM_WALL);

    for(int i = 0; i < NUM_BOX_WIDTH; i++)
    {
        for(int j = 0; j < NUM_BOX_HEIGHT; j++)
        {
            box[i][j].setrect(BOX_SIZE, BOX_SIZE * i, BOX_SIZE * j,2);
            //box[i][j].setSize(BOX_SIZE);                        //every box has the same size
            //box[i][j].setPosition(BOX_SIZE * i, BOX_SIZE * j);  //set the position of the boxes
        }
    }


/*
    currentbox: a pointer for marking the box that the cursor located.
    lastbox:    a pointer that remember the last 'currentbox'.
    When the game started, lastbox is pointing to nowhere.
*/
    Box *currentbox, *lastbox;

    lastbox = NULL;
/*
    player_score_order[][]: each player's score box order.
    use Set_Score_Pool to randomize the score box order.
*/
    int player_score_order[NUM_PLAYER][NUM_SCORE_EACH_PLAYER];
    Set_Score_Pool(player_score_order, NUM_SCORE_EACH_PLAYER);

/*******************************************************************
    The main game start here.
********************************************************************/
    //create the window
    sf::RenderWindow window(sf::VideoMode(NUM_BOX_WIDTH * BOX_SIZE, NUM_BOX_HEIGHT * BOX_SIZE), "Prototype", sf::Style::Close);

    while (window.isOpen())
    {
        sf::Event event;

        //---------------------------------------------
        //Graphing here
        //---------------------------------------------
        window.clear();
        for(int i = 0; i < NUM_BOX_WIDTH; i++)
        {
            for(int j = 0; j < NUM_BOX_HEIGHT;j++)
            {
                box[i][j].show(&window);

                if(box[i][j].getscore() != 0)
                window.draw(score[i][j]);
            }
        }
        window.display();

        //----------------------------------------------
        //Event here
        //----------------------------------------------
        if(window.waitEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseMoved)
            {
                //get the position of the mouse, make sure the mouse is in the window!
                if(sf::Mouse::getPosition(window).x > 0
                   && sf::Mouse::getPosition(window).y > 0
                   && sf::Mouse::getPosition(window).x < NUM_BOX_WIDTH * BOX_SIZE
                   && sf::Mouse::getPosition(window).y < NUM_BOX_HEIGHT * BOX_SIZE)
                mouseposition = sf::Mouse::getPosition(window);
                //get the current box
                currentbox = &box[mouseposition.x / BOX_SIZE][mouseposition.y / BOX_SIZE];
                if(currentbox != lastbox)
                {
                    Show_Cursor_CurrentBox(currentbox,lastbox);
                    lastbox = currentbox;
                }
            }

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                //cpature the box, return true if success, failed if the box is owned or the box is a wall.
                if(currentbox->capture_directly_by(active_player,player_score_order[active_player][turn_count]))
                {
                    //update the graphing score
                    char buf[256];
                    sprintf(buf, "%d",  player_score_order[active_player][turn_count]);
                    score[mouseposition.x / BOX_SIZE][mouseposition.y / BOX_SIZE].setString(buf);

                    //attack or strengthen the nearby boxes
                    Check_adjacent(mouseposition, currentbox, box, score);

                    //next player move
                    active_player++;
                    //if it is the last player of that turn, reset to player1.
                    if(active_player == NUM_PLAYER)
                    {
                        active_player = 0;
                        turn_count++;               //next turn.
                    }
                }
            }
        }
    }
    system("pause");
    return 0;
}
/*
Show_Cursor_CurrentBox funcion
This funcion changes the color of box when cursor moved to a new box.
*/
void Show_Cursor_CurrentBox(Box *currentbox, Box *lastbox)
{
        //the current box turns cyan
        if(currentbox->getowner() == 5)
        {
            currentbox->setFillColor(sf::Color::Cyan);
        }
        //the lastbox changes to white, if the lastbox is occupied, don't chnange it to white.
        if(lastbox !=NULL && lastbox->getowner() == 5)
        {
            lastbox->setFillColor(sf::Color::White);
        }
}

/*
Set_Wall funcion
Pick some walls and turn it to wall.
THe numofwall variable must be less than the total number of box in the game.
*/
void Set_Wall(Box box[][NUM_BOX_HEIGHT],int numofwall)
{
    int count = 0;
    while(count < numofwall)
    {
        int target1 = rand() % NUM_BOX_WIDTH;
        int target2 = rand() % NUM_BOX_HEIGHT;
        if(box[target1][target2].getowner() != 4)
        {
            box[target1][target2].setwall();
            count++;
        }
    }
}
/*
set score pool funcion
generate score pool, assign to each player with randomized order.
*/
void Set_Score_Pool(int player_score_order[NUM_PLAYER][NUM_SCORE_EACH_PLAYER],int num)
{
    //Generate score pool here
    for(int i = 0; i < num; i++)
    {
        int randomnum = rand() % 25 + 5;
        for(int j = 0; j < NUM_PLAYER; j++)
        {
            player_score_order[j][i] = randomnum;
        }
    }
    //assign to each player with randomized order
    for(int i = 0; i < NUM_PLAYER; i++)
    {
        for(int j = 0; j < 100; j++)
        {
            int randomnum1 = rand() % NUM_SCORE_EACH_PLAYER;
            int hold = player_score_order[i][randomnum1];
            int randomnum2 = rand() % NUM_SCORE_EACH_PLAYER;
            player_score_order[i][randomnum1] = player_score_order[i][randomnum2];
            player_score_order[i][randomnum2] = hold;
        }
    }
    //print the player score for debug
    std::cout << "Debug:\n";
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < NUM_SCORE_EACH_PLAYER; j++)
        {
            std::cout << i << " , " << j << " = " << player_score_order[i][j] << "\n";
        }
    }
}
/*Check_adjacent funcion
This function check the box nearby and capture them (or fortify them). Then update the score for graphing,
noted that the player and the box score are already updated in box->check(Box) function.
IMPORTANT: Before calling the box->check(Box) function, check if the box is at the edge to prevent out of array error.

*/
void Check_adjacent(sf::Vector2i mouseposition,Box *currentbox, Box box[][NUM_BOX_WIDTH], sf::Text score[][NUM_BOX_HEIGHT])
{
    //up
    if( currentbox != &box[mouseposition.x / BOX_SIZE][0] && (currentbox - 1)->check(currentbox))
    {
        //update the graphing score
        char buf[256];
        sprintf(buf, "%d",(currentbox - 1)->getscore());
        score[mouseposition.x / BOX_SIZE][(mouseposition.y / BOX_SIZE) - 1].setString(buf);
    }
    //bottom
    if( currentbox != &box[mouseposition.x / BOX_SIZE][NUM_BOX_HEIGHT - 1] && (currentbox + 1)->check(currentbox))
    {
        char buf[256];
        sprintf(buf, "%d",(currentbox + 1)->getscore());
        score[mouseposition.x / BOX_SIZE][(mouseposition.y / BOX_SIZE) + 1].setString(buf);
    }
    //left
    if( currentbox != &box[0][mouseposition.y / BOX_SIZE] && (currentbox - NUM_BOX_HEIGHT)->check(currentbox))
    {
        char buf[256];
        sprintf(buf, "%d",(currentbox - NUM_BOX_HEIGHT)->getscore());
        score[(mouseposition.x / BOX_SIZE) - 1][mouseposition.y / BOX_SIZE].setString(buf);
    }
    //right
    if( currentbox != &box[NUM_BOX_WIDTH - 1][mouseposition.y / BOX_SIZE] && (currentbox + NUM_BOX_HEIGHT)->check(currentbox))
    {
        char buf[256];
        sprintf(buf, "%d",(currentbox + NUM_BOX_HEIGHT)->getscore());
        score[(mouseposition.x / BOX_SIZE) + 1][mouseposition.y / BOX_SIZE].setString(buf);
    }
    //left top
    if( currentbox != &box[mouseposition.x / BOX_SIZE][0]
        && currentbox != &box[0][mouseposition.y / BOX_SIZE]
        && (currentbox - NUM_BOX_HEIGHT - 1)->check(currentbox))
    {
        char buf[256];
        sprintf(buf, "%d",(currentbox - NUM_BOX_HEIGHT - 1)->getscore());
        score[(mouseposition.x / BOX_SIZE) - 1][(mouseposition.y / BOX_SIZE) - 1].setString(buf);
    }
    //left bottom
    if( currentbox != &box[0][mouseposition.y / BOX_SIZE]
        && currentbox != &box[mouseposition.x / BOX_SIZE][NUM_BOX_HEIGHT - 1]
        && (currentbox - NUM_BOX_HEIGHT + 1)->check(currentbox))
    {
        char buf[256];
        sprintf(buf, "%d",(currentbox - NUM_BOX_HEIGHT + 1)->getscore());
        score[(mouseposition.x / BOX_SIZE) - 1][(mouseposition.y / BOX_SIZE) + 1].setString(buf);
    }
    //right top
    if( currentbox != &box[NUM_BOX_WIDTH - 1][mouseposition.y / BOX_SIZE]
        && currentbox != &box[mouseposition.x / BOX_SIZE][0]
        && (currentbox + NUM_BOX_HEIGHT - 1)->check(currentbox))
    {
        char buf[256];
        sprintf(buf, "%d",(currentbox + NUM_BOX_HEIGHT - 1)->getscore());
        score[(mouseposition.x / BOX_SIZE) + 1][(mouseposition.y / BOX_SIZE) - 1].setString(buf);
    }
    //right bottom
    if( currentbox != &box[NUM_BOX_WIDTH - 1][mouseposition.y / BOX_SIZE]
        && currentbox != &box[mouseposition.x / BOX_SIZE][NUM_BOX_HEIGHT - 1]
        && (currentbox + NUM_BOX_HEIGHT + 1)->check(currentbox))
    {
        char buf[256];
        sprintf(buf, "%d",(currentbox + NUM_BOX_HEIGHT + 1)->getscore());
        score[(mouseposition.x / BOX_SIZE) + 1][(mouseposition.y / BOX_SIZE) + 1].setString(buf);
    }
}

