#ifndef SCENE_H
#define SCENE_H
#include <string>
#include <functional>

class Scene
{   
    public:
        Scene(std::string name);
        ~Scene();
        std::function<void(double)> update;
        std::function<void()> init;
};

std::string GetSceneName(Scene *scene);
void SetCurrentScene(Scene* scene);
void SetCurrentSceneByName(std::string name);
extern Scene *current_scene;
#endif