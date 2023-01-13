#include <Game/Atlas.hpp>
#include <SFML/Graphics.hpp>
#include <RapidXML/rapidxml_utils.hpp>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <array>

std::string removeNumbers(std::string str)
{
    int current = 0;
    for(int i = 0; i< str.length(); i++){
        if(!isdigit(str[i])){
            str[current] = str[i];
            current++;
        }
    }

    return str.substr(0,current);
}

void Atlas::LoadPNG(std::string path)
{
    text.loadFromFile(path + ".png");
    sprite.setTexture(text);
    size = sf::Vector2f(sprite.getLocalBounds().width, sprite.getLocalBounds().height);
    sprite.setPosition(0, 50);

    rapidxml::file<> file((path + ".xml").c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(file.data());

    rapidxml::xml_node<>* node = doc.first_node();

        for (rapidxml::xml_node<> *child = node->first_node(); child; 
        child = child->next_sibling())
    {
        const char* x = child->first_attribute("x")->value();
        const char *y = child->first_attribute("y")->value();
        const char *width = child->first_attribute("width")->value();
        const char *height = child->first_attribute("height")->value();
        std::string name = removeNumbers(child->first_attribute("name")->value());
        int x2, y2, w, h;
        std::stringstream strVal;
        strVal << x; strVal >> x2;
        std::stringstream strVal2;
        strVal2 << y; strVal2 >> y2;
        std::stringstream strVal3;
        strVal3 << width; strVal3 >> w;
        std::stringstream strVal4;
        strVal4 << height; strVal4 >> h;

        if (regions.find(name) == regions.end())
        { regions[name] = {}; }

        regions[name].insert(regions[name].end(), sf::IntRect(x2, y2, w, h));
    }

    delete node;
}

void Atlas::Update()
{
    if (this->spr_clock.getElapsedTime().asMilliseconds() > 1000.0 / this->speed)
    {
        if (regions.find(animation) != regions.end())
        {
            this->spr_clock.restart();
            this->frame += 1;

            if (regions[animation.c_str()].size() - 1 < frame)
            {
                this->frame = 0;
            }
        }
    }

    sprite.setTextureRect(regions[animation][frame]);
}

void Atlas::Draw(sf::RenderWindow& win)
{
    win.draw(this->sprite);
}

void Atlas::SetSize(sf::Vector2f resize)
{
    sprite.setScale(sf::Vector2f(resize.x/size.x,resize.y/size.y));
    size = resize;
}

void Atlas::Play(std::string name)
{
    if (regions.find(name.c_str()) == regions.end()) { return ;}
    animation = name;
    frame = 0;
    spr_clock.restart();
}

void Atlas::SetPosition(sf::Vector2f position)
{
    sprite.setPosition(position);
}