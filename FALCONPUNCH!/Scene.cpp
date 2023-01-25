#include <Game/Scene.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

std::vector<Scene*> scenes;
Scene* current_scene;

Scene* FindScene(std::string name)
{
    for (Scene* scene : scenes)
    {
        if (scene->GetName() == name) return scene;
    }

    return NULL;
}

void SetCurrentScene(Scene* scene)
{ 
    current_scene = scene;
    
    if (current_scene->init)
    {
        current_scene->init();
    }
}

void SetCurrentSceneByName(std::string name)
{ 
    if (FindScene(name)) SetCurrentScene(FindScene(name));
}

Scene::Scene(std::string name)
{
    if (FindScene(name)) throw std::invalid_argument("Scene with name " + name + " already existed!");
    this->name = name; scenes.push_back(this);
}

Scene::Scene(std::string name, std::function<void()> init, std::function<void(double)> update)
{
    if (FindScene(name)) throw std::invalid_argument("Scene with name " + name + " already existed!");    
    this->init = init; this->update = update;
    this->name = name; scenes.push_back(this);
}

Scene::~Scene()
{
    scenes.erase(
        std::remove(
            scenes.begin(), scenes.end(), this
        ), scenes.end()
    );
}

const std::string Scene::GetName()
{
    return this->name;
}

void UpdateScene(double delta)
{
    if (current_scene)
    {
        if (current_scene->update)
        {
            current_scene->update(delta);
        }
    }
}