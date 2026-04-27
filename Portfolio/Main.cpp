#define SDL_MAIN_HANDLED 

#include "Minigin.h"
#include <iostream>

#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include <utility>
#include <filesystem>
#include <SDL3/SDL_scancode.h>
#include <MoveCommand.h>
#include <InputManager.h>
#include "TriggerComponent.h"

namespace fs = std::filesystem;

void BindPlayerInputs(dae::GameObject* playerPtr)
{
    auto& input = dae::InputManager::GetInstance();
    input.UnbindAll();

    float playerSpeed = 150.0f;

    // WASD
    input.BindCommand(SDL_SCANCODE_W, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, -1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_S, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, 1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_A, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ -1, 0 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_D, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 1, 0 }, playerSpeed));

    // Arrows
    input.BindCommand(SDL_SCANCODE_UP, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, -1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_DOWN, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, 1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_LEFT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ -1, 0 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_RIGHT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 1, 0 }, playerSpeed));
}

void load()
{
    std::cout << "Welcome to the Portfolio!\n";
    auto& sceneManager = dae::SceneManager::GetInstance();

// MAIN MENU

    auto& mainScene = sceneManager.CreateScene();

    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>("MainMenuBackground.png");
    mainScene.Add(std::move(background));

    auto player1 = std::make_unique<dae::GameObject>();
    player1->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    player1->SetLocalPosition(642.5f, 400.0f);
    auto player1Ptr = player1.get();
    mainScene.Add(std::move(player1));

    auto treeTop = std::make_unique<dae::GameObject>();
    treeTop->AddComponent<dae::RenderComponent>("TreesTop.png");
    treeTop->SetLocalPosition(1366.0f - 446.0f, 768.0f - 324.0f);
    mainScene.Add(std::move(treeTop));

    // Trigger to go to ABOUT
    auto triggerToAbout = std::make_unique<dae::GameObject>();
    triggerToAbout->SetLocalPosition((1366.0f / 2.0f) - (125.0f / 2.0f), 0);
    auto tComp1 = triggerToAbout->AddComponent<dae::TriggerComponent>(125.0f, 60.0f);
    tComp1->SetTarget(player1Ptr, 88.0f, 120.0f);
    mainScene.Add(std::move(triggerToAbout));

// ABOUT

    auto& aboutScene = sceneManager.CreateScene();

    auto aboutBg = std::make_unique<dae::GameObject>();
    aboutBg->AddComponent<dae::RenderComponent>("AboutBackground.png");
    aboutScene.Add(std::move(aboutBg));

    auto player2 = std::make_unique<dae::GameObject>();
    player2->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    auto player2Ptr = player2.get();
    aboutScene.Add(std::move(player2));

    // Trigger to go back to MAIN MENU
    auto triggerToMain = std::make_unique<dae::GameObject>();
    triggerToMain->SetLocalPosition((1366.0f / 2.0f) - (125.0f / 2.0f), 768.0f - 25.0f);
    auto tComp2 = triggerToMain->AddComponent<dae::TriggerComponent>(150.0f, 25.0f);
    tComp2->SetTarget(player2Ptr, 88.0f, 120.0f);
    aboutScene.Add(std::move(triggerToMain));


// LINK THE TRIGGERS

    tComp1->SetOnTriggerEnter([player2Ptr]()
        {
            std::cout << "Going to About Scene...\n";
            dae::InputManager::GetInstance().UnbindAll();

            dae::SceneManager::GetInstance().TransitionToScene(1, [player2Ptr]()
                {
                    player2Ptr->SetLocalPosition(642.5f, 768.0f - 160.0f);
                    BindPlayerInputs(player2Ptr);
                });
        });

    tComp2->SetOnTriggerEnter([player1Ptr]()
        {
            std::cout << "Going to Main Scene...\n";
            dae::InputManager::GetInstance().UnbindAll();

            dae::SceneManager::GetInstance().TransitionToScene(0, [player1Ptr]()
                {
                    player1Ptr->SetLocalPosition(642.5f, 95.0f);
                    BindPlayerInputs(player1Ptr);
                });
        });

// START THE GAME

    BindPlayerInputs(player1Ptr);
    sceneManager.SetActiveScene(0);
}

int main(int, char* [])
{
    std::cout << "Starting Portfolio Engine...\n";

#ifdef __EMSCRIPTEN__
    fs::path data_location = "";
#else
    fs::path data_location = "./Data/";
    if (!fs::exists(data_location))
    {
        data_location = "../Data/";
    }
#endif

    dae::Minigin engine(data_location);
    engine.Run(load);

    return 0;
}