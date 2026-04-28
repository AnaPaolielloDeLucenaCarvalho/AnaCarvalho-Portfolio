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
#include <MiniaudioSoundSystem.h>
#include <LoggingSoundSystem.h>
#include "ServiceLocator.h"

namespace fs = std::filesystem;

void BindPlayerInputs(dae::GameObject* playerPtr, const std::vector<SDL_FRect>& walkableZones = {})
{
    auto& input = dae::InputManager::GetInstance();
    input.UnbindAll();

    float playerSpeed = 150.0f;

    // WASD 
    input.BindCommand(SDL_SCANCODE_W, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, -1 }, playerSpeed, walkableZones));
    input.BindCommand(SDL_SCANCODE_S, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, 1 }, playerSpeed, walkableZones));
    input.BindCommand(SDL_SCANCODE_A, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ -1, 0 }, playerSpeed, walkableZones));
    input.BindCommand(SDL_SCANCODE_D, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 1, 0 }, playerSpeed, walkableZones));

    // Arrows
    input.BindCommand(SDL_SCANCODE_UP, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, -1 }, playerSpeed, walkableZones));
    input.BindCommand(SDL_SCANCODE_DOWN, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, 1 }, playerSpeed, walkableZones));
    input.BindCommand(SDL_SCANCODE_LEFT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ -1, 0 }, playerSpeed, walkableZones));
    input.BindCommand(SDL_SCANCODE_RIGHT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 1, 0 }, playerSpeed, walkableZones));
}


void load()
{
    std::cout << "Welcome to the Portfolio!\n";

    std::string dataPath = "";
#ifdef __EMSCRIPTEN__
    dataPath = "Data/";
#else
    if (std::filesystem::exists("./Data/")) dataPath = "./Data/";
    else dataPath = "../Data/";
#endif

	// Initialize sound system and play background music
    auto audioSystem = std::make_unique<dae::MiniaudioSoundSystem>();
    dae::ServiceLocator::register_sound_system(std::make_unique<dae::LoggingSoundSystem>(std::move(audioSystem)));
    dae::ServiceLocator::get_sound_system().loadSound(0, dataPath + "AnimalCrossingNewHorizonsMainTheme.mp3");
    dae::ServiceLocator::get_sound_system().play(0, 0.25);

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
    treeTop->AddComponent<dae::RenderComponent>("MainMenuTop.png");
    treeTop->SetLocalPosition(159.0f, 442.0f);
    mainScene.Add(std::move(treeTop));

	// Main Menu -> About
    auto triggerToAbout = std::make_unique<dae::GameObject>();
    triggerToAbout->SetLocalPosition(624.0f, 0);
    auto tComp1 = triggerToAbout->AddComponent<dae::TriggerComponent>(124.0f, 48.0f);
    tComp1->SetTarget(player1Ptr, 88.0f, 120.0f);
    mainScene.Add(std::move(triggerToAbout));

	// Main Menu -> Contact
    auto triggerToContact = std::make_unique<dae::GameObject>();
    triggerToContact->SetLocalPosition(1280.0f, 388.0f);
    auto tComp2 = triggerToContact->AddComponent<dae::TriggerComponent>(86.0f, 168.0f);
    tComp2->SetTarget(player1Ptr, 88.0f, 120.0f);
    mainScene.Add(std::move(triggerToContact));

	// Main Menu -> Projects
    auto triggerToProjects = std::make_unique<dae::GameObject>();
    triggerToProjects->SetLocalPosition(624.0f, 684.0f);
    auto tComp3 = triggerToProjects->AddComponent<dae::TriggerComponent>(124.0f, 84.0f);
    tComp3->SetTarget(player1Ptr, 88.0f, 120.0f);
    mainScene.Add(std::move(triggerToProjects));

// ABOUT

    auto& aboutScene = sceneManager.CreateScene();

    auto aboutBg = std::make_unique<dae::GameObject>();
    aboutBg->AddComponent<dae::RenderComponent>("AboutBackground.png");
    aboutScene.Add(std::move(aboutBg));

    auto player2 = std::make_unique<dae::GameObject>();
    player2->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    auto player2Ptr = player2.get();
    aboutScene.Add(std::move(player2));

	// About -> Main Menu
    auto triggerToMain1 = std::make_unique<dae::GameObject>();
    triggerToMain1->SetLocalPosition(624.0f, 720.0f);
    auto tComp4 = triggerToMain1->AddComponent<dae::TriggerComponent>(124.0f, 48.0f);
    tComp4->SetTarget(player2Ptr, 88.0f, 120.0f);
    aboutScene.Add(std::move(triggerToMain1));

// CONTACT

    auto& contactScene = sceneManager.CreateScene();

    auto contactBg = std::make_unique<dae::GameObject>();
    contactBg->AddComponent<dae::RenderComponent>("ContactBackground.png");
    contactScene.Add(std::move(contactBg));

    auto player3 = std::make_unique<dae::GameObject>();
    player3->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    auto player3Ptr = player3.get();
    contactScene.Add(std::move(player3));

	// Contact -> Main Menu
    auto triggerToMain2 = std::make_unique<dae::GameObject>();
    triggerToMain2->SetLocalPosition(0, 388.0f);
    auto tComp5 = triggerToMain2->AddComponent<dae::TriggerComponent>(86.0f, 168.0f);
    tComp5->SetTarget(player3Ptr, 88.0f, 120.0f);
    contactScene.Add(std::move(triggerToMain2));

// PROJECTS
  
    auto& projectsScene = sceneManager.CreateScene();

    auto projectsBg = std::make_unique<dae::GameObject>();
    projectsBg->AddComponent<dae::RenderComponent>("ProjectsBackground.png");
    projectsScene.Add(std::move(projectsBg));

    auto player4 = std::make_unique<dae::GameObject>();
    player4->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    auto player4Ptr = player4.get();
    projectsScene.Add(std::move(player4));

	// Projects -> Main Menu
    auto triggerToMain3 = std::make_unique<dae::GameObject>();
    triggerToMain3->SetLocalPosition(624.0f, 0.0f);
    auto tComp6 = triggerToMain3->AddComponent<dae::TriggerComponent>(124.0f, 84.0f);
    tComp6->SetTarget(player4Ptr, 88.0f, 120.0f);
    projectsScene.Add(std::move(triggerToMain3));


// TRIGGERS

    std::vector<SDL_FRect> mainScenePlanks = 
    {
        SDL_FRect{ 632.0f, 0.0f, 108.0f, 768.0f }, // Vertical
        SDL_FRect{ 740.0f, 448.0f, 626.0f, 92.0f } // Horizontal
    };

    std::vector<SDL_FRect> aboutScenePlanks =
    {
        SDL_FRect{ 632.0f, 484.0f, 108.0f, 284.0f } // Vertical
    };

    std::vector<SDL_FRect> contactScenePlanks =
    {
        SDL_FRect{ 0.0f, 460.0f, 612.0f, 92.0f } // Horizontal
    };

	// Main Menu -> About
    tComp1->SetOnTriggerEnter([player2Ptr, aboutScenePlanks]() {
        std::cout << "Going to About Scene...\n";
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(1, [player2Ptr, aboutScenePlanks]() {
            player2Ptr->SetLocalPosition(642.5f, 768.0f - 160.0f);
            BindPlayerInputs(player2Ptr, aboutScenePlanks);
            });
        });

	// Main Menu -> Contact
    tComp2->SetOnTriggerEnter([player3Ptr, contactScenePlanks]() {
        std::cout << "Going to Contact Scene...\n";
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(2, [player3Ptr, contactScenePlanks]() {
            player3Ptr->SetLocalPosition(150.0f, 400.0f);
            BindPlayerInputs(player3Ptr, contactScenePlanks);
            });
        });

	// Main Menu -> Projects
    // The only one the player can walk around!
    tComp3->SetOnTriggerEnter([player4Ptr]() {
        std::cout << "Going to Projects Scene...\n";
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(3, [player4Ptr]() {
            player4Ptr->SetLocalPosition(642.5f, 95.0f);
            BindPlayerInputs(player4Ptr);
            });
        });

	// About -> Main Menu
    tComp4->SetOnTriggerEnter([player1Ptr, mainScenePlanks]() {
        std::cout << "Going to Main Scene...\n";
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(0, [player1Ptr, mainScenePlanks]() {
            player1Ptr->SetLocalPosition(642.5f, 95.0f);
            BindPlayerInputs(player1Ptr, mainScenePlanks);
            });
        });

	// Contact -> Main Menu
    tComp5->SetOnTriggerEnter([player1Ptr, mainScenePlanks]() {
        std::cout << "Going to Main Scene...\n";
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(0, [player1Ptr, mainScenePlanks]() {
            player1Ptr->SetLocalPosition(1366.0f - 250.0f, 400.0f);
            BindPlayerInputs(player1Ptr, mainScenePlanks);
            });
        });

	// Projects -> Main Menu
    tComp6->SetOnTriggerEnter([player1Ptr, mainScenePlanks]() {
        std::cout << "Going to Main Scene...\n";
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(0, [player1Ptr, mainScenePlanks]() {
            player1Ptr->SetLocalPosition(642.5f, 768.0f - 250.0f);
            BindPlayerInputs(player1Ptr, mainScenePlanks);
            });
        });

// START GAME

    BindPlayerInputs(player1Ptr, mainScenePlanks);
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