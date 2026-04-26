#define SDL_MAIN_HANDLED 

#include "Minigin.h"
#include <iostream>

#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include <utility>

void load()
{
    std::cout << "Welcome to the Portfolio!\n";

    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>("MainMenu Background.png");
    background->SetLocalPosition(0, 0);
    scene.Add(std::move(background));
}

int main(int, char* [])
{
    std::cout << "Starting Portfolio Engine...\n";

    // Start the engine
    dae::Minigin engine("./Data/");
    engine.Run(load);

    return 0;
}