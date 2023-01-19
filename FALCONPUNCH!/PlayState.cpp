#include <Game/Scene.hpp>
#include <Game/Playstate.hpp>
#include <Game/Globals.hpp>
#include <iostream>
#include <fstream>
#include <SFML/Audio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <Game/Note.hpp>
#include <Game/Atlas.hpp>

using json = nlohmann::json;

std::vector<Note> notes;
std::vector<Lane> lanes;

std::string song_name;

sf::Clock play_clock;

sf::View hud_view;

sf::Music vocals;
sf::Music inst;

sf::Text hudText;

double conductor_position = 0;
const double conductor_In_secs = conductor_position / 1000;
double last_beat_position = 0;
double health_bar_width = 800;
double maxHealth = 1.0;
double health = 0.5;
double hud_zoom;
double BPM;

int beat_count;

void init()
{
    conductor_position = 0;
    last_beat_position = 0;
    beat_count = 0;
    health = 0.5;

    hud_view.setCenter(win_size.x / 2, win_size.y / 2);
    hud_view.setSize(sf::Vector2f(win_size));
    inst.openFromFile("assets/Inst.ogg");
    vocals.openFromFile("assets/Voices.ogg");

    json chart_data = json::parse(std::fstream("assets/chart.json"));
    song_name = chart_data["song"]["song"];
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
            lane.note_animation = "purple";
        }
        else if (index == 1 || index == 5)
        {
            lane.default_animation = "arrowDOWN";
            lane.hit_animation = "down confirm";
            lane.note_animation = "blue";
        }
        else if (index == 2 || index == 6)
        {
            lane.default_animation = "arrowUP";
            lane.hit_animation = "up confirm";
            lane.note_animation = "green";
        }
        else if (index == 3 || index == 7)
        {
            lane.default_animation = "arrowRIGHT";
            lane.hit_animation = "right confirm";
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


    window.setTitle("FalconPunch! - " + song_name);
    play_clock.restart();

//  This made arrows not spawn so don't mess with this 

  //  while (play_clock.getElapsedTime().asSeconds() < 3.0);
    play_clock.restart();
    inst.play();
    vocals.play();
}

auto desktop = sf::VideoMode::getDesktopMode();

void update(double delta)
{
    const double crochet = 60.0 / BPM;
    conductor_position = play_clock.getElapsedTime().asMilliseconds();
    window.setView(hud_view);

    if (conductor_position > 88000 && conductor_position < 109000)
    {
        window.setPosition(
            sf::Vector2i(
                (desktop.width / 2) - (win_size.x / 2) - (cos((conductor_position / 1000) / (crochet / 3)) * 8),
                (desktop.height / 2) - (win_size.y / 2) - (abs(sin((conductor_position / 1000) / (crochet / 3))) * 16)));
    }

    for (Lane &lane : lanes)
    {
        lane.hit = false;
        if (!lane.bot)
        {
            if (sf::Keyboard::isKeyPressed(lane.key) && !lane.already_hit)
            {
                lane.already_hit = true;
                lane.hit = true;
            } else if (!sf::Keyboard::isKeyPressed(lane.key) && lane.already_hit)
            {
                lane.already_hit = false;
            }
        }

        if (!lane.atlas.IsPlaying())
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
        if ((abs(note_position) < 75 && !lane.bot && lane.hit) || (note_position <= 0 && lane.bot))
        {
            lane.atlas.PlayAnim(lane.hit_animation);
            note.hit = true;

            if (!lane.bot) health += 0.01; else health -= 0.01;
            continue;
        }

        if (!note.atlas.IsPlaying())
        {
            note.atlas.PlayAnim(lane.note_animation);
        }

        note.atlas.position = lane.atlas.position + sf::Vector2f(0, note_position);
        note.atlas.Update();
        note.atlas.Draw(window);
    }

    health = std::max(0.0, std::min(health, 1.0));
    
    sf::RectangleShape healthbar;
    healthbar.setPosition(sf::Vector2f(win_size.x/2, 680));
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
    healthbar.setPosition(sf::Vector2f(win_size.x / 2 + (health_bar_width*1.5) - (health_bar_width * health), 680));
    window.draw(healthbar);

    hud_zoom = std::min(1.0, hud_zoom + (1 - hud_zoom) * 0.1);
    hud_view.setSize(sf::Vector2f(win_size.x * hud_zoom, win_size.y * hud_zoom));
}

Scene PlayState("Playstate", init, update);