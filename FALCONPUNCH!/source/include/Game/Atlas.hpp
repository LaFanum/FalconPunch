#ifndef ATLAS_H
#define ATLAS_H
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include <array>
#include <iostream>

class AtlasFrame
{
    public:
        double x;
        double y;
        double w;
        double h;
        double fx;
        double fy;
        double fw;
        double fh;
};

class Atlas
{
    private:
        int frame_ = 0;
        bool playing_ = false;

        std::string texture;
        std::string animation_;

        sf::Clock clock;
    public:
        sf::Vector2f position;
        sf::Vector2f scale = {1.0f, 1.0f};

        double speed = 10.0;

        bool centered;
        bool loop;

        Atlas();
        ~Atlas();

        void SetTexture(std::string filename);
        void PlayAnim(std::string animation);
        void Update(); void Draw(sf::RenderTarget &target);
        const std::string GetAnimation() { return this->animation_; }
        const bool IsPlaying() { return this->playing_; }
};

#endif