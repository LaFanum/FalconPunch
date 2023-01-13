#include <Game/Scene.hpp>
#include <string>
#include <map>

std::map<std::string, Scene*> scenes;
Scene *current_scene;

std::string GetSceneName(Scene* scene)
{
    for (auto it = scenes.begin(); it != scenes.end(); it++)
    { if (it->second == scene) { return it->first; } }
}

void SetCurrentScene(Scene* scene)
{ 
    current_scene = scene; 

    if (current_scene->init) { current_scene->init(); }
}

void SetCurrentSceneByName(std::string name)
{ 
    if (scenes.find(name) == scenes.end()) { return; }
    SetCurrentScene(scenes[name]);
}

Scene::Scene(std::string name)
{ scenes[name] = this; }

Scene::~Scene()
{ scenes.erase(GetSceneName(this));}