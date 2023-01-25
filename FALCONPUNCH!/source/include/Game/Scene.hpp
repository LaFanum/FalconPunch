#ifndef SCENE_H
#define SCENE_H
#include <string>
#include <functional>

class Scene
{   
    private:
        std::string name;
    public:
        const std::string GetName();
        Scene(std::string name, std::function<void()> init, std::function<void(double)> update);
        Scene(std::string name);
        ~Scene();
        std::function<void(double)> update;
        std::function<void()> init;
};

void SetCurrentScene(Scene* scene);
void SetCurrentSceneByName(std::string name);
void UpdateScene(double delta);
#endif