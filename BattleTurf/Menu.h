#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Ingame.h"
#include "Game_State.h"
#include "Graphic_button.h"

enum Menu_state{mainmenu = 1,
                setting1 = 2, setting2 = 3, setting3 = 4,
                multiplayer1 = 5, multiplayer_client = 6, multiplayer_lobby_host = 7, multiplayer_lobby_client = 8,
                end_ingame = 10, end_multiplayer_ingame = 11,
                terminated = 12};

class Game_Menu : public Game_State
{
private:

    //the background of menu
    sf::RectangleShape menu_background;

    //mainmenu
    Graphic_button* startButton;
    Graphic_button* multiPlayerButton;
    Graphic_button* exitButton;

    //settings
    Graphic_button* nextButton;
    Graphic_button* backButton;

    //multiplayer1
    Graphic_button* hostButton;
    Graphic_button* clientButton;

    //multiplayer_client
    Graphic_button* connectButton;

    //multiplayer_lobby
    sf::RectangleShape lobby_Player_Color[4];
    sf::RectangleShape lobby_Player_Bar[4];

    //the (testing) image of menu
    sf::Texture gameTitle;
    sf::RectangleShape gameTitleRect;

    //sf::RectangleShape menubutton_exit;
    //sf::Text menubutton_exit_text;
    Graphic_button* menubutton_exit;

    //debug text
    sf::Text debug_menustate;

    //input ip
    sf::Text Input_IP;
    sf::String IP;
    sf::Text IP_Text;
    sf::RectangleShape IP_Bar;

    Menu_state menu_state;

    void setting_nextButton();
    void setting_backButton();

    //Handle the events
    void Mouseclicked();

    void Mousemoved();

    //convert the user input to char
    void Modified_IP();

    /**Networking**/
    //tcp socket
    sf::TcpSocket* ptrsocket;
    //tcp listener
    sf::TcpListener* ptrlistener;
    //try to connect to host
    bool TryConnet();
    //open host listener
    bool StartListen();
    //update function
    void network_update();

public:
    //constructor
    Game_Menu();
    Game_Menu(sf::RenderWindow *window, sf::Event *event, sf::Vector2i *mouseposition, Game_data *ptrsetting, sf::Font *font, sf::TcpSocket* socket, sf::TcpListener* listener);

    //destructor
    ~Game_Menu();

    //update the graphic
    void update();

    Menu_state getMenu_state();

    //Handle Event
    void HandleEvent();
};

#endif // MENU_H_INCLUDED
