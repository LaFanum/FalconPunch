#include <iostream>
#include <Game/Globals.hpp>
#include <Game/Scene.hpp>
#include <Game/TitleState.hpp>
#include <Game/Playstate.hpp>
#include <Game/Atlas.hpp>
#include <SFML/Window.hpp>

int main(int argc, const char *argv[])
{
    InitializeGlobals();
    SetCurrentScene(&TitleState);

    while (window.isOpen())
    {
        sf::Event event;
        double delta = game_clock.restart().asSeconds();
        
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed: window.close(); break;
            }
        }

        if (fadeDir == 1) {
            fadeProgress += delta;

            if (fadeProgress > 1)
            {
                fadeProgress = 1;
                fadeDir = 0;
            }
        }
        else if (fadeDir == 2) {
            fadeProgress -= delta;

            if (fadeProgress < -3)
            {
                fadeProgress = -3;
                fadeDir = 0;
            }
        }

        UpdateScene(delta);
        fadeSpr.setPosition(0, fadeProgress * win_size.y);
        window.draw(fadeSpr);
        window.display();
    }

    QuitGlobals();
    return EXIT_SUCCESS;
}