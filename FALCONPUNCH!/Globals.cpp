#include <Game/Globals.hpp>
#include <iostream>
#include <Windows.h>

sf::RenderWindow window;
sf::Clock game_clock;
sf::Vector2u win_size;

int InitializeGlobals()
{
    window.create(sf::VideoMode(1280, 720), "....");
    window.setFramerateLimit(60);
    win_size = window.getSize();
}

void QuitGlobals()
{
}