#include <Game/Scene.hpp>
#include <Game/Playstate.hpp>
#include <Game/Globals.hpp>
#include <iostream>
#include <fstream>
#include <SFML/Audio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <tuple>
#include <Game/Note.hpp>
#include <Game/Atlas.hpp>

using json = nlohmann::json;

std::vector<Note> notes;
std::vector<Lane> lanes;
std::vector<std::tuple<sf::Sprite, sf::Vector2f, double>> fadingSprites;

std::string song_name;

sf::Clock play_clock;
sf::Clock pause_clock;

sf::View hud_view;

sf::Music vocals;
sf::Music inst;

sf::Text hudText;
sf::Text gameVersion;

sf::Texture ratingImages[5];
sf::Texture numImages[10];

double conductor_position = 0;
double conductor_In_secs = 0;
double last_beat_position = 0;
double health_bar_width = 800;
double maxHealth = 1.0;
double health = 0.5;
double scroll_speed = 1.0;
double hud_zoom;
double BPM;

int score;
int misses;
int beat_count;
int combo;

void init()
{
    conductor_position = 0;
    last_beat_position = 0;
    beat_count = 0;
    score = 0;
    misses = 0;
    combo = 0;
    health = 0.5;
    scroll_speed = 1.0;

    hud_view.setCenter(win_size.x / 2, win_size.y / 2);
    hud_view.setSize(sf::Vector2f(win_size));
 //   hudText.setFont(phantommuff);
    inst.openFromFile("assets/Inst.ogg");
    vocals.openFromFile("assets/Voices.ogg");

    ratingImages[0].loadFromFile("assets/images/sick.png");
    ratingImages[1].loadFromFile("assets/images/good.png");
    ratingImages[2].loadFromFile("assets/images/bad.png");
    ratingImages[4].loadFromFile("assets/images/combo.png");

    for (int i = 0; i < 10; i++)
    {
        numImages[i].loadFromFile("assets/images/num" + std::to_string(i) + ".png");
   }

    json chart_data = json::parse(std::fstream("assets/chart.json"));
    song_name = chart_data["song"]["song"];
    scroll_speed = chart_data["song"]["speed"];
    BPM = chart_data["song"]["bpm"];

    for (auto it = chart_data["song"]["notes"].begin(); it != chart_data["song"]["notes"].end(); it++)
    {
        for (auto i : it.value()["sectionNotes"])
        {
            Note note;
            note.position = i[0];
            note.length = i[2];

            int type = i[1];

            if (it.value()["mustHitSection"])
            {
                if (i[1] > 3) {note.type = type - 4;}
                else if (i[1] < 4) {note.type = type + 4;}
            } else {note.type = type;}
            
            note.atlas.centered = true;
            note.atlas.SetTexture("assets/images/NOTES");
            note.atlas.scale = {0.75, 0.75};
            notes.push_back(note);
        }
    }

    for (int index = 0; index < 8; index++)
    {   
        Lane lane;
        lane.atlas.scale = {0.75, 0.75};
        lane.atlas.centered = true;
        lane.bot = false;
        lane.atlas.SetTexture("assets/images/NOTES");

        if (index < 4) lane.bot = true;
        if (index == 0 || index == 4) 
        {
            lane.default_animation = "arrowLEFT";
            lane.hit_animation = "left confirm";
            lane.hit_animation_fake = "left press";
            lane.note_animation = "purple";
        }
        else if (index == 1 || index == 5)
        {
            lane.default_animation = "arrowDOWN";
            lane.hit_animation = "down confirm";
            lane.hit_animation_fake = "down press";
            lane.note_animation = "blue";
        }
        else if (index == 2 || index == 6)
        {
            lane.default_animation = "arrowUP";
            lane.hit_animation = "up confirm";
            lane.hit_animation_fake = "up press";
            lane.note_animation = "green";
        }
        else if (index == 3 || index == 7)
        {
            lane.default_animation = "arrowRIGHT";
            lane.hit_animation = "right confirm";
            lane.hit_animation_fake = "right press";
            lane.note_animation = "red";
        }

        lanes.push_back(lane);
    }
    
    lanes[0].atlas.position = sf::Vector2f(100, 100);
    lanes[1].atlas.position = sf::Vector2f(210, 100);
    lanes[2].atlas.position = sf::Vector2f(320, 100);
    lanes[3].atlas.position = sf::Vector2f(430, 100);
    lanes[4].atlas.position = sf::Vector2f(850, 100);
    lanes[5].atlas.position = sf::Vector2f(960, 100);
    lanes[6].atlas.position = sf::Vector2f(1070, 100);
    lanes[7].atlas.position = sf::Vector2f(1180, 100);

    lanes[4].key = sf::Keyboard::D;
    lanes[5].key = sf::Keyboard::F;
    lanes[6].key = sf::Keyboard::J;
    lanes[7].key = sf::Keyboard::K;

    window.setTitle("FalconPunch - " + song_name);
    play_clock.restart();
    inst.play();
    vocals.play();
}

auto desktop = sf::VideoMode::getDesktopMode();

void update(double delta)
{
    const double crochet = 60.0 / BPM;
    conductor_position = play_clock.getElapsedTime().asMilliseconds();
    conductor_In_secs = play_clock.getElapsedTime().asSeconds();
    window.setView(hud_view);

    for (Lane &lane : lanes)
    {
        lane.hit = false;
        if (!lane.bot)
        {
            if (sf::Keyboard::isKeyPressed(lane.key) && !lane.already_hit)
            {
                lane.atlas.PlayAnim(lane.hit_animation_fake);
                lane.already_hit = true;
                lane.hit = true;
            } else if (!sf::Keyboard::isKeyPressed(lane.key) && lane.already_hit)
            {
                lane.already_hit = false;
            }
        }

        if (!lane.atlas.IsPlaying() && !sf::Keyboard::isKeyPressed(lane.key))
        {
            lane.atlas.PlayAnim(lane.default_animation);
        }

        lane.atlas.Update();
        lane.atlas.Draw(window);
    }

    if (conductor_position / 1000 > last_beat_position + crochet)
    {
        last_beat_position += crochet;
        beat_count += 1;

        if (beat_count % 8 == 0) hud_zoom = 0.95;
    }

    for (Note& note : notes)
    {
        Lane &lane = lanes[note.type];
        
        double note_position = note.position - conductor_position;

        if (note.hit || note.missed) continue;
        if ((abs(note_position) < 200 && !lane.bot && lane.hit) || (note_position <= 0 && lane.bot))
        {
            lane.atlas.PlayAnim(lane.hit_animation);
            note.hit = true;

            if (!lane.bot) {
                std::tuple<sf::Sprite, sf::Vector2f, double> fadingSpr;
                std::tuple<sf::Sprite, sf::Vector2f, double> comboSpr;
                double hit_time = abs(note_position);
                health += 0.01;
                lane.hit = false;
                combo++;

                std::get<0>(comboSpr).setTexture(ratingImages[4]);
                
                //Yandare ass code lmao
                if (hit_time <= 33.33)
                { score += 350; std::get<0>(fadingSpr).setTexture(ratingImages[0]); } 
                else if (hit_time <= 91.67 && hit_time > 33.33)
                { score += 200; std::get<0>(fadingSpr).setTexture(ratingImages[1]); } 
                else if (hit_time <= 133.33 && hit_time > 91.67)
                { score += 100; std::get<0>(fadingSpr).setTexture(ratingImages[2]);  } 
                else if (hit_time > 133.33 ) 
                { score += 50; std::get<0>(fadingSpr).setTexture(ratingImages[3]); }

                std::get<0>(fadingSpr).setScale(0.5, 0.5);
                std::get<0>(comboSpr).setScale(0.5, 0.5);
                std::get<1>(fadingSpr) = {win_size.x / 2 - 25, 75};
                std::get<1>(comboSpr) = {win_size.x / 2 - 25, 125};
                std::get<2>(fadingSpr) = conductor_In_secs;
                std::get<2>(comboSpr) = conductor_In_secs;
                fadingSprites.push_back(fadingSpr);
                fadingSprites.push_back(comboSpr);

                std::string combo_str = std::to_string(combo);
                size_t len = combo_str.size();

                if (len < 2) {combo_str = "00" + combo_str;}
                else if (len < 3) {combo_str = "0" + combo_str; }

                len = combo_str.size();
                int index = 0;
                for (char character : combo_str)
                {
                    std::tuple<sf::Sprite, sf::Vector2f, double> numSpr;
                    int number = character - '0';

                    std::get<0>(numSpr).setTexture(numImages[number]);
                    std::get<0>(numSpr).setScale(0.5, 0.5);
                    std::get<1>(numSpr) = {win_size.x / 2 - (len * 40 / 2) + (index * 40) + 25, 175};
                    std::get<2>(numSpr) = conductor_In_secs;

                    fadingSprites.push_back(numSpr);
                    index++;
                }
            } else health -= 0.01;
        } else if (note_position < -200 && !lane.bot)
        {
            combo = 0;
            note.missed = true;
            health -= 0.01;
            misses++;
        }

        if (!note.atlas.IsPlaying())
        {
            note.atlas.PlayAnim(lane.note_animation);
        }

        note.atlas.position = lane.atlas.position + sf::Vector2f(0, note_position * (scroll_speed / 3.0));
        note.atlas.Update();
        note.atlas.Draw(window);
    }

    health = std::max(0.0, std::min(health, 1.0));
    
    sf::RectangleShape healthbar;
    healthbar.setPosition(sf::Vector2f(win_size.x/2, 670));
    healthbar.setOrigin(sf::Vector2f(health_bar_width/2, 8));
    healthbar.setSize(sf::Vector2f(health_bar_width, 16));
    healthbar.setFillColor(sf::Color::Red);
    healthbar.setOutlineColor(sf::Color::Black);
    healthbar.setOutlineThickness(4.0);
    window.draw(healthbar);

    healthbar.setOutlineThickness(0);
    healthbar.setFillColor(sf::Color::Green);
    healthbar.setSize(sf::Vector2f(health_bar_width * health, 16));
    healthbar.setOrigin(sf::Vector2f(health_bar_width, 8));
    healthbar.setPosition(sf::Vector2f(win_size.x / 2 + (health_bar_width*1.5) - (health_bar_width * health), healthbar.getPosition().y));
    window.draw(healthbar);

    hudText.setCharacterSize(24);
    hudText.setColor(sf::Color::White);
    hudText.setOutlineColor(sf::Color::Black);
    hudText.setOutlineThickness(4);
    hudText.setString("Score: " + std::to_string(score) + " -- Misses: " + std::to_string(misses));
    hudText.setPosition(sf::Vector2f(win_size.x / 2, 680));
    hudText.setOrigin(hudText.getLocalBounds().width/2, 0);
    window.draw(hudText);

    int it = 0;
    for (auto tuple : fadingSprites)
    {
        const sf::Texture *texture = std::get<0>(tuple).getTexture();
        sf::Vector2f scale = std::get<0>(tuple).getScale();
        sf::Vector2f size = (sf::Vector2f)(texture->getSize());
        double time = conductor_In_secs - std::get<2>(tuple);
        double height = sin(time * 8) * -32;
        std::get<0>(tuple).setPosition(std::get<1>(tuple) + sf::Vector2f(0, height));
        std::get<0>(tuple).setOrigin(sf::Vector2f(size.x * scale.x, size.y * scale.y) / 2.f);
        std::get<0>(tuple).setColor(sf::Color(255, 255, 255, (std::get<2>(tuple) - conductor_In_secs) / 0.5 * 255));
        window.draw(std::get<0>(tuple));

        if (time > 0.45) fadingSprites.erase(fadingSprites.begin() + it);
        it++;
    }

    hud_zoom = std::min(1.0, hud_zoom + (1 - hud_zoom) * 0.1);
    hud_view.setSize(sf::Vector2f(win_size.x * hud_zoom, win_size.y * hud_zoom));
}

Scene PlayState("Playstate", init, update);