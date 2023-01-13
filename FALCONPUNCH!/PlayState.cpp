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

Scene PlayState("PlayState");
std::vector<Note> notes;
std::string song_name;
sf::Clock play_clock;
sf::Clock beat_clock;
sf::View hud_view;
sf::Music vocals;
sf::Music inst;
Atlas note_spr;

double conductor_position = 0;
double hud_zoom;
int old_beat;
int BPM;

bool already_beat;

void init()
{
    conductor_position = 0;
    inst.openFromFile("assets/Inst.ogg");
    vocals.openFromFile("assets/Vocals.ogg");
    json chart_data = json::parse(std::fstream("assets/bubbles.json"));
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

            notes.push_back(note);
        }
    }

    window.setTitle("FalconPunch!  - " + song_name);
    play_clock.restart();
    beat_clock.restart();
    inst.play();
    vocals.play();
}

void update(double delta)
{
    conductor_position = play_clock.getElapsedTime().asMilliseconds();
    window.setView(hud_view);
    note_spr.Play("arrowLEFT");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280/10, 35));
    note_spr.Draw(window);

    note_spr.Play("arrowDOWN");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280/10+120, 35));
    note_spr.Draw(window);

    note_spr.Play("arrowUP");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280/10+240, 35));
    note_spr.Draw(window);

    note_spr.Play("arrowRIGHT");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280 / 10 + 360, 35));
    note_spr.Draw(window);


    note_spr.Play("arrowLEFT");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280 / 2, 35));
    note_spr.Draw(window);

    note_spr.Play("arrowDOWN");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280 / 2 + 120, 35));
    note_spr.Draw(window);

    note_spr.Play("arrowUP");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280 / 2 + 240, 35));
    note_spr.Draw(window);

    note_spr.Play("arrowRIGHT");
    note_spr.Update();
    note_spr.SetPosition(sf::Vector2f(1280 / 2 + 360, 35));
    note_spr.Draw(window);

    for (Note note : notes)
    {
        double note_pos = note.position - (conductor_position);
        if (note_pos < 0) {continue;}

        switch (note.type)
        {
            case 0:
                note_spr.Play("purple");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 10, 35 + note_pos));
                note_spr.Draw(window);
                break;
            case 1:
                note_spr.Play("blue");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 10+120, 35 + note_pos));
                note_spr.Draw(window);
                break;
            case 2:
                note_spr.Play("green");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 10 + 240, 35 + note_pos));
                note_spr.Draw(window);
                break;
            case 3:
                note_spr.Play("red");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 10 + 360, 35 + note_pos));
                note_spr.Draw(window);
                break;
            case 4:
                note_spr.Play("purple");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 2, 35 + note_pos));
                note_spr.Draw(window);
                break;
            case 5:
                note_spr.Play("blue");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 2 + 120, 35 + note_pos));
                note_spr.Draw(window);
                break;
            case 6:
                note_spr.Play("green");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 2 + 240, 35 + note_pos));
                note_spr.Draw(window);
                break;
            case 7:
                note_spr.Play("red");
                note_spr.Update();
                note_spr.SetPosition(sf::Vector2f(1280 / 2 + 360, 35 + note_pos));
                note_spr.Draw(window);
                break;
        }
    }

    int beat = (int)(play_clock.getElapsedTime().asSeconds() * BPM / 60.0) % 4;

    if (old_beat != beat && beat == 1 && !already_beat)
    {
        already_beat = true;
        hud_zoom = 0.9;
    }
    
    if (old_beat != beat && beat != 1) 
    {already_beat = false;}

    old_beat = beat;
    hud_zoom = std::min(1.0, hud_zoom + (1 - hud_zoom) * 0.1);
    hud_view.setSize(sf::Vector2f(win_size.x * hud_zoom, win_size.y * hud_zoom));
}

void InitPlayState()
{
    PlayState.init = init;
    PlayState.update = update;
    note_spr.LoadPNG("assets/images/NOTES");
    note_spr.SetSize(sf::Vector2f(1500, 750));
    hud_view.setCenter(win_size.x/2, win_size.y/2);
    hud_view.setSize(sf::Vector2f(win_size));
}