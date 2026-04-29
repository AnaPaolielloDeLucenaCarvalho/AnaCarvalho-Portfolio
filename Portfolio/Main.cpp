#define SDL_MAIN_HANDLED 

#include "Minigin.h"
#include <iostream>
#include <vector>
#include <functional>

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
#include "MiniaudioSoundSystem.h"
#include "LoggingSoundSystem.h"
#include "ServiceLocator.h"
#include <chrono>

namespace fs = std::filesystem;

// INTERACTION SYSTEM
std::vector<std::pair<dae::TriggerComponent*, std::function<void()>>> g_ProjectInteractions;

bool g_IsMuted = false;
std::vector<std::pair<dae::GameObject*, dae::GameObject*>> g_SoundIcons;

class ActionCommand : public dae::Command
{
    std::function<void()> m_Action;
public:
    ActionCommand(std::function<void()> action) : m_Action(action) {}
    void Execute(float /*deltaTime*/) override { if (m_Action) m_Action(); }
};

void ToggleMuteGlobal()
{
    static auto lastToggleTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTime).count() < 250) {
        return;
    }
    lastToggleTime = currentTime;

    dae::ServiceLocator::get_sound_system().ToggleMute();
    g_IsMuted = !g_IsMuted;

    for (auto& icons : g_SoundIcons)
    {
        if (g_IsMuted)
        {
            icons.first->SetLocalPosition(-2000.0f, -2000.0f);
            icons.second->SetLocalPosition(1306.0f, 32.0f);
        }
        else
        {
            icons.first->SetLocalPosition(1306.0f, 20.0f);
            icons.second->SetLocalPosition(-2000.0f, -2000.0f);
        }
    }
}

void AddMuteIconsToScene(dae::Scene& scene)
{
    auto soundOn = std::make_unique<dae::GameObject>();
    soundOn->AddComponent<dae::RenderComponent>("SoundOn.png");
    auto soundOnPtr = soundOn.get();

    auto soundOff = std::make_unique<dae::GameObject>();
    soundOff->AddComponent<dae::RenderComponent>("SoundOff.png");
    auto soundOffPtr = soundOff.get();

    auto f2 = std::make_unique<dae::GameObject>();
    f2->AddComponent<dae::RenderComponent>("F2.png");
    f2->SetLocalPosition(1250.0f, 22.0f);

    if (g_IsMuted)
    {
        soundOnPtr->SetLocalPosition(-2000.0f, -2000.0f);
        soundOffPtr->SetLocalPosition(1306.0f, 32.0f);
    }
    else
    {
        soundOnPtr->SetLocalPosition(1306.0f, 20.0f);
        soundOffPtr->SetLocalPosition(-2000.0f, -2000.0f);
    }

    g_SoundIcons.push_back({ soundOnPtr, soundOffPtr });

    scene.Add(std::move(soundOn));
    scene.Add(std::move(soundOff));
    scene.Add(std::move(f2));
}

// INPUT BINDINGS
void BindPlayerInputs(dae::GameObject* playerPtr, const std::vector<SDL_FRect>& walkableZones = {}, bool canInteract = false)
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

    // F2 to Mute
    input.BindCommand(SDL_SCANCODE_F2, dae::KeyState::Pressed, std::make_unique<ActionCommand>(ToggleMuteGlobal));

    // "E" to interact
    if (canInteract) 
    {
        input.BindCommand(SDL_SCANCODE_E, dae::KeyState::Pressed, std::make_unique<ActionCommand>([]()
            {
                for (auto& interaction : g_ProjectInteractions)
                {
                    if (interaction.first->IsInside())
                    {
                        interaction.second();
                        break;
                    }
                }
            }));
    }
}

// Inside Project (ESC to leave, Q/E carousels)
void BindProjectViewInputs(std::function<void()> onEsc, std::function<void()> onQ, std::function<void()> onE)
{
    auto& input = dae::InputManager::GetInstance();
	input.UnbindAll(); // No WASD/Arrows

    input.BindCommand(SDL_SCANCODE_ESCAPE, dae::KeyState::Pressed, std::make_unique<ActionCommand>(onEsc));
    input.BindCommand(SDL_SCANCODE_Q, dae::KeyState::Pressed, std::make_unique<ActionCommand>(onQ));
    input.BindCommand(SDL_SCANCODE_E, dae::KeyState::Pressed, std::make_unique<ActionCommand>(onE));

    // F2 to Mute
    input.BindCommand(SDL_SCANCODE_F2, dae::KeyState::Pressed, std::make_unique<ActionCommand>(ToggleMuteGlobal));
}

// MAIN SCENE
void LoadMainMenu(dae::GameObject*& outPlayer, dae::TriggerComponent*& tAbout, dae::TriggerComponent*& tContact, dae::TriggerComponent*& tProj)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto bg = std::make_unique<dae::GameObject>();
    bg->AddComponent<dae::RenderComponent>("MainMenuBackground.png");
    scene.Add(std::move(bg));

    auto player = std::make_unique<dae::GameObject>();
    player->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    player->SetLocalPosition(642.5f, 400.0f);
    outPlayer = player.get();
    scene.Add(std::move(player));

    auto treeTop = std::make_unique<dae::GameObject>();
    treeTop->AddComponent<dae::RenderComponent>("MainMenuTop.png");
    treeTop->SetLocalPosition(159.0f, 442.0f);
    scene.Add(std::move(treeTop));

    // Triggers
    auto tr1 = std::make_unique<dae::GameObject>();
    tr1->SetLocalPosition(624.0f, 0);
    tAbout = tr1->AddComponent<dae::TriggerComponent>(124.0f, 48.0f);
    tAbout->SetTarget(outPlayer, 88.0f, 120.0f);
    scene.Add(std::move(tr1));

    auto tr2 = std::make_unique<dae::GameObject>();
    tr2->SetLocalPosition(1280.0f, 388.0f);
    tContact = tr2->AddComponent<dae::TriggerComponent>(86.0f, 168.0f);
    tContact->SetTarget(outPlayer, 88.0f, 120.0f);
    scene.Add(std::move(tr2));

    auto tr3 = std::make_unique<dae::GameObject>();
    tr3->SetLocalPosition(624.0f, 684.0f);
    tProj = tr3->AddComponent<dae::TriggerComponent>(124.0f, 84.0f);
    tProj->SetTarget(outPlayer, 88.0f, 120.0f);
    scene.Add(std::move(tr3));

    AddMuteIconsToScene(scene);
}

// ABOUT
void LoadAboutScene(dae::GameObject*& outPlayer, dae::TriggerComponent*& tMain)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto bg = std::make_unique<dae::GameObject>();
    bg->AddComponent<dae::RenderComponent>("AboutBackground.png");
    scene.Add(std::move(bg));

    auto player = std::make_unique<dae::GameObject>();
    player->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    outPlayer = player.get();
    scene.Add(std::move(player));

    auto tr1 = std::make_unique<dae::GameObject>();
    tr1->SetLocalPosition(624.0f, 720.0f);
    tMain = tr1->AddComponent<dae::TriggerComponent>(124.0f, 48.0f);
    tMain->SetTarget(outPlayer, 88.0f, 120.0f);
    scene.Add(std::move(tr1));

    AddMuteIconsToScene(scene);
}

// CONTACT
void LoadContactScene(dae::GameObject*& outPlayer, dae::TriggerComponent*& tMain)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto bg = std::make_unique<dae::GameObject>();
    bg->AddComponent<dae::RenderComponent>("ContactBackground.png");
    scene.Add(std::move(bg));

    auto player = std::make_unique<dae::GameObject>();
    player->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    outPlayer = player.get();
    scene.Add(std::move(player));

    auto tr1 = std::make_unique<dae::GameObject>();
    tr1->SetLocalPosition(0, 388.0f);
    tMain = tr1->AddComponent<dae::TriggerComponent>(86.0f, 168.0f);
    tMain->SetTarget(outPlayer, 88.0f, 120.0f);
    scene.Add(std::move(tr1));

    AddMuteIconsToScene(scene);
}

// PROJECT SCENE GENERATOR
void CreateSingleProjectScene(dae::TriggerComponent* flowerTrigger, const std::string& bgName, dae::GameObject* projectsPlayerPtr, int targetSceneIndex)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto bg = std::make_unique<dae::GameObject>();
    bg->AddComponent<dae::RenderComponent>(bgName);
    scene.Add(std::move(bg));

    auto onEsc = [projectsPlayerPtr]() 
        {
        std::cout << "Returning to Projects...\n";

        dae::SceneManager::GetInstance().TransitionToScene(3, [projectsPlayerPtr]()
            {
                BindPlayerInputs(projectsPlayerPtr, {}, true);
            });
        };

    auto onQ = []() { std::cout << "Future Carousel Left!\n"; };
    auto onE = []() { std::cout << "Future Carousel Right!\n"; };

    g_ProjectInteractions.push_back({ flowerTrigger, [targetSceneIndex, onEsc, onQ, onE]()
    {
        std::cout << "Loading Project Screen...\n";

        dae::SceneManager::GetInstance().TransitionToScene(targetSceneIndex, [onEsc, onQ, onE]()
        {
                BindProjectViewInputs(onEsc, onQ, onE);
            });
        } });

    AddMuteIconsToScene(scene);
}

// PROJECTS
void LoadProjectsScene(dae::GameObject*& outPlayer, dae::TriggerComponent*& tMain)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto bg = std::make_unique<dae::GameObject>();
    bg->AddComponent<dae::RenderComponent>("ProjectsBackground.png");
    scene.Add(std::move(bg));

    auto player = std::make_unique<dae::GameObject>();
    player->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    outPlayer = player.get();
    scene.Add(std::move(player));

    auto trMain = std::make_unique<dae::GameObject>();
    trMain->SetLocalPosition(624.0f, 0.0f);
    tMain = trMain->AddComponent<dae::TriggerComponent>(124.0f, 84.0f);
    tMain->SetTarget(outPlayer, 88.0f, 120.0f);
    scene.Add(std::move(trMain));

    auto popupObj = std::make_unique<dae::GameObject>();
    popupObj->AddComponent<dae::RenderComponent>("PressE.png");
    popupObj->SetLocalPosition(-2000.0f, -2000.0f);
    auto popupPtr = popupObj.get();

    // 6 PROJECTS
    struct ProjectInfo {
        glm::vec2 triggerPos;
        glm::vec2 popupPos;
        std::string bgImageName;
    };

    // { Trigger X/Y , Popup X/Y , Background Image }
    std::vector<ProjectInfo> projects = {
        { {692.0f, 484.0f}, {884.0f, 620.0f}, "Proj1_Bg.png" },
        { {301.0f, 309.0f}, {488.0f, 445.0f}, "Proj2_Bg.png" },
        { {8.0f, 29.0f}, {196.0f, 160.0f}, "Proj3_Bg.png" },
        { {40.0f, 524.0f}, {217.0f, 648.0f}, "Proj4_Bg.png" },
        { {789.0f, 45.0f}, {964.0f, 184.0f}, "Proj5_Bg.png" },
        { {908.0f, 292.0f}, {1130.0f, 436.0f}, "Proj6_Bg.png" }
    };

    for (size_t i = 0; i < projects.size(); ++i)
    {
        auto flowerObj = std::make_unique<dae::GameObject>();
        flowerObj->SetLocalPosition(projects[i].triggerPos.x, projects[i].triggerPos.y);
        auto tComp = flowerObj->AddComponent<dae::TriggerComponent>(223.0f, 223.0f);
        tComp->SetTarget(outPlayer, 88.0f, 120.0f);

        glm::vec2 customPopupPos = projects[i].popupPos;

        tComp->SetOnTriggerEnter([popupPtr, customPopupPos]() 
            {
            popupPtr->SetLocalPosition(customPopupPos.x, customPopupPos.y);
            });

        tComp->SetOnTriggerExit([popupPtr]() 
            {
            popupPtr->SetLocalPosition(-2000.0f, -2000.0f);
            });

        CreateSingleProjectScene(tComp, projects[i].bgImageName, outPlayer, static_cast<int>(4 + i));
        scene.Add(std::move(flowerObj));
    }

    scene.Add(std::move(popupObj));
    AddMuteIconsToScene(scene);
}

// LOAD GAME & MAIN
void load()
{
    std::cout << "Welcome to the Portfolio!\n";

    std::string dataPath = "";
#ifdef __EMSCRIPTEN__
    dataPath = "";
#else
    if (std::filesystem::exists("./Data/")) dataPath = "./Data/";
    else dataPath = "../Data/";
#endif

    // Audio
    auto audioSystem = std::make_unique<dae::MiniaudioSoundSystem>();
    dae::ServiceLocator::register_sound_system(std::make_unique<dae::LoggingSoundSystem>(std::move(audioSystem)));
    dae::ServiceLocator::get_sound_system().loadSound(0, dataPath + "AnimalCrossingNewHorizonsMainTheme.mp3");
    dae::ServiceLocator::get_sound_system().play(0, 0.25);

    dae::GameObject* p1, * p2, * p3, * p4;
    dae::TriggerComponent* tMainToAbout, * tMainToContact, * tMainToProj;
    dae::TriggerComponent* tAboutToMain, * tContactToMain, * tProjToMain;

    // Build the Main 4 Scenes
    LoadMainMenu(p1, tMainToAbout, tMainToContact, tMainToProj);
    LoadAboutScene(p2, tAboutToMain);
    LoadContactScene(p3, tContactToMain);
    LoadProjectsScene(p4, tProjToMain);

    // TRIGGERS & PLANKS
    std::vector<SDL_FRect> mainScenePlanks =
    {
        SDL_FRect{ 632.0f, 0.0f, 108.0f, 768.0f },
        SDL_FRect{ 740.0f, 448.0f, 626.0f, 92.0f }
    };
    std::vector<SDL_FRect> aboutScenePlanks = { SDL_FRect{ 632.0f, 484.0f, 108.0f, 284.0f } };
    std::vector<SDL_FRect> contactScenePlanks = { SDL_FRect{ 0.0f, 460.0f, 612.0f, 92.0f } };

    // Main -> About
    tMainToAbout->SetOnTriggerEnter([p2, aboutScenePlanks]() 
        {
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(1, [p2, aboutScenePlanks]() 
            {
            p2->SetLocalPosition(642.5f, 768.0f - 160.0f);
            BindPlayerInputs(p2, aboutScenePlanks);
            });
        });

    // Main -> Contact
    tMainToContact->SetOnTriggerEnter([p3, contactScenePlanks]() 
        {
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(2, [p3, contactScenePlanks]() 
            {
            p3->SetLocalPosition(150.0f, 400.0f);
            BindPlayerInputs(p3, contactScenePlanks);
            });
        });

    // Main -> Projects
    tMainToProj->SetOnTriggerEnter([p4]() 
        {
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(3, [p4]() 
            {
            p4->SetLocalPosition(642.5f, 95.0f);
            BindPlayerInputs(p4, {}, true);
            });
        });

    // About -> Main
    tAboutToMain->SetOnTriggerEnter([p1, mainScenePlanks]() 
        {
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(0, [p1, mainScenePlanks]() 
            {
            p1->SetLocalPosition(642.5f, 95.0f);
            BindPlayerInputs(p1, mainScenePlanks);
            });
        });

    // Contact -> Main
    tContactToMain->SetOnTriggerEnter([p1, mainScenePlanks]() 
        {
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(0, [p1, mainScenePlanks]() 
            {
            p1->SetLocalPosition(1366.0f - 250.0f, 400.0f);
            BindPlayerInputs(p1, mainScenePlanks);
            });
        });

    // Projects -> Main
    tProjToMain->SetOnTriggerEnter([p1, mainScenePlanks]() 
        {
        dae::InputManager::GetInstance().UnbindAll();
        dae::SceneManager::GetInstance().TransitionToScene(0, [p1, mainScenePlanks]() 
            {
            p1->SetLocalPosition(642.5f, 768.0f - 250.0f);
            BindPlayerInputs(p1, mainScenePlanks);
            });
        });

    // START GAME
    BindPlayerInputs(p1, mainScenePlanks);
    dae::SceneManager::GetInstance().SetActiveScene(0);
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