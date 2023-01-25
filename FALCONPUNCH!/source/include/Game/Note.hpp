#ifndef NOTE_H
#define NOTE_H
#include <Game/Atlas.hpp>
#include <SFML/Window.hpp>

class Note
{
    public:
        Atlas atlas;
        double position;
        bool missed = false;
        bool hit = false;
        int type;
        int length;
};

class Lane
{
    public:
        sf::Keyboard::Key key;
        std::string default_animation;
        std::string hit_animation_fake;
        std::string hit_animation;
        std::string note_animation;
        Atlas atlas;

        bool bot = false;
        bool already_hit;
        bool hit;
};

#endif