#include <Game/Atlas.hpp>
#include <SFML/Graphics.hpp>
#include <RapidXML/rapidxml_utils.hpp>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <array>

std::map<std::string, sf::Texture> textures;
std::map<std::string, std::map<std::string, std::vector<AtlasFrame>>> texture_regions;
sf::Sprite masterSprite;

std::string removeNumbersOnStr(std::string str)
{
    std::string result;

    for (char chr : str)
    {
        if (!std::isdigit(chr)) {
            result += chr;
        }
    }

    return result;
}

void Atlas::SetTexture(std::string filename)
{
    if (textures.find(filename) == textures.end())
    {
        sf::Texture loadedTexture;
        std::map<std::string, std::vector<AtlasFrame>> loadedRegions;
        rapidxml::file<> xmlFile((filename + ".xml").c_str());
        rapidxml::xml_document<> doc;
        loadedTexture.loadFromFile(filename + ".png");
        doc.parse<0>(xmlFile.data());

        rapidxml::xml_node<>* textureAtlas = doc.first_node();
        
        for (
            rapidxml::xml_node<> *node = textureAtlas->first_node(); 
            node != NULL; node = node->next_sibling()
        )
        {
            std::string nameWoutNums = removeNumbersOnStr(node->first_attribute("name")->value());
            AtlasFrame frame;

            frame.x = std::stod(node->first_attribute("x")->value());
            frame.y = std::stod(node->first_attribute("y")->value());
            frame.w = std::stod(node->first_attribute("width")->value());
            frame.h = std::stod(node->first_attribute("height")->value());

            if (node->first_attribute("frameX")) {
                frame.fx = std::stod(node->first_attribute("frameX")->value());
                frame.fy = std::stod(node->first_attribute("frameY")->value());
                frame.fw = std::stod(node->first_attribute("frameWidth")->value());
                frame.fh = std::stod(node->first_attribute("frameHeight")->value());
            }

            if (loadedRegions.find(nameWoutNums) == loadedRegions.end()) loadedRegions[nameWoutNums] = {};
            loadedRegions[nameWoutNums].push_back(frame);
        }

        texture_regions[filename] = loadedRegions;
        textures[filename] = loadedTexture;
    }

    texture = filename;
}

void Atlas::PlayAnim(std::string animation)
{
    if (
        texture_regions[this->texture].find(animation) == texture_regions[this->texture].end() ||
        this->texture == ""
    )
        return;
    
    this->playing_ = true;
    this->animation_ = animation;
    this->frame_ = 0;
    this->clock.restart();
}

void Atlas::Update()
{

    if (this->clock.getElapsedTime().asSeconds() > 1.0 / this->speed && this->playing_)
    {
        this->frame_++;
        this->clock.restart();

        if (texture_regions[this->texture][this->animation_].size() >= this->frame_)
        {
            this->playing_ = this->loop;
            
            if (this->playing_) this->frame_ = 0; else this->frame_--;
        }
    }
}

void Atlas::Draw(sf::RenderTarget &target)
{
    AtlasFrame frame = texture_regions[this->texture][this->animation_][this->frame_];
    sf::Vector2f pos = this->position - sf::Vector2f(frame.fx, frame.fy);

    if (this->centered)
    {
        masterSprite.setOrigin(sf::Vector2f(frame.w/2, frame.h/2));
    }

    masterSprite.setPosition(pos);
    masterSprite.setScale(this->scale);
    masterSprite.setTexture(textures[this->texture]);
    masterSprite.setTextureRect(sf::IntRect(frame.x, frame.y, frame.w, frame.h));
    target.draw(masterSprite);
}

Atlas::Atlas()
{
}

Atlas::~Atlas()
{
}