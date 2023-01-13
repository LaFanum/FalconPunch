#include <iostream>
#include <Game/Globals.hpp>
#include <Game/Scene.hpp>
#include <Game/Playstate.hpp>
#include <SFML/Window.hpp>


int main()
{
    InitializeGlobals();
    InitPlayState();
    SetCurrentScene(&PlayState);

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

        window.clear(sf::Color::Transparent);
        if (current_scene->update) {current_scene->update(delta);}
        window.display();
    }

    QuitGlobals();
    return EXIT_SUCCESS;
}