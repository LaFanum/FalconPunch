#include <Game/Globals.hpp>
#include <iostream>

sf::RenderWindow window;
sf::Clock game_clock;
sf::Vector2u win_size;
sf::Font phantommuff;
sf::Sprite fadeSpr;
sf::Texture fadeTexture;
sf::Image fadeImg;
float fadeProgress;
int fadeDir = 0;

int InitializeGlobals()
{
    window.create(sf::VideoMode(1280, 720), "FalconPunch");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    win_size = window.getSize();
    phantommuff.loadFromFile("assets/fonts/phantommuff.ttf");
    fadeDir = 0;
    fadeProgress = -3;

    fadeImg.create(win_size.x, win_size.y * 3);

    for (int y = 0; y < win_size.y * 2; y++)
    {
        for (int x = 0; win_size.x > x; x++)
        {
            fadeImg.setPixel(x, y, sf::Color(0, 0, 0,  
                std::min(float(y) / float(win_size.y), 1.f) * 255)
            );
        }
    }

    for (int y = win_size.y * 2; win_size.y * 3 > y; y++)
    {
        for (int x = 0; win_size.x > x; x++)
        {
            fadeImg.setPixel(x, y, sf::Color(0, 0, 0,  
                (float(win_size.y * 3) - float(y)) / float(win_size.y) * 255)
            );
        }
    }
    
    fadeTexture.loadFromImage(fadeImg);
    fadeSpr.setTexture(fadeTexture);
}

void QuitGlobals()
{
}