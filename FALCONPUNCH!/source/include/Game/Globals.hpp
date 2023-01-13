#ifndef GLOBALS_H
#define GLOBALS_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

extern sf::RenderWindow window;
extern sf::Clock game_clock;
extern sf::Vector2u win_size;
int InitializeGlobals();
void QuitGlobals();
#endif  