#ifndef ATLAS_H
#define ATLAS_H
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include <array>

class Atlas
{
    private:
        sf::Vector2f size;
        sf::Texture text;
        sf::Sprite sprite;
        sf::Clock spr_clock;
        std::array<int, 4> region;
        std::map < std::string, std::vector<sf::IntRect>> regions;
        int frame;
    public:
        int speed = 1;
        std::string animation;
        void Update();
        void LoadPNG(std::string path);
        void Draw(sf::RenderWindow& win);
        void SetSize(sf::Vector2f resize);
        void Play(std::string name);
        void SetPosition(sf::Vector2f position);
};

#endif