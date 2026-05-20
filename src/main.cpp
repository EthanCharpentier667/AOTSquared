/*
** EPITECH PROJECT, 2026
** EngineSquared
** File description:
** main
*/

#include "Raylib.hpp"
#include "plugin/Core.hpp"
#include "plugin/PluginScene.hpp"
#include "resource/SceneManager.hpp"
#include "scene/TestScene.hpp"
#include "system/ChildFollowParentSystem.hpp"

void Setup(Engine::Core &core) {
    core.GetResource<Scene::Resource::SceneManager>()
        .RegisterScene<aot::plugin::scene::TestScene>("TestScene");
    core.GetResource<Scene::Resource::SceneManager>().SetNextScene("TestScene");
    core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
        ChildFollowParentSystem);
}

int main(void) {
    Engine::Core core;

    core.AddPlugins<aot::plugin::raylib::RaylibPlugin, aot::plugin::Core,
                    Scene::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>(Setup);

    core.Run();

    return 0;
}
