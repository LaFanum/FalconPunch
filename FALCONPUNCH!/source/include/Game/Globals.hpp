#ifndef GLOBALS_H
#define GLOBALS_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

extern sf::RenderWindow window;
extern sf::Clock game_clock;
extern sf::Vector2u win_size;
extern sf::Font phantommuff;
extern sf::Sprite fadeSpr;
extern float fadeProgress;
extern int fadeDir;
int InitializeGlobals();
void QuitGlobals();
#endif  