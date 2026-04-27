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

namespace fs = std::filesystem;

void load()
{
    std::cout << "Welcome to the Portfolio!\n";

    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    // BACKGROUND
    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>("MainMenuBackground.png");
    background->SetLocalPosition(0, 0);
    scene.Add(std::move(background));

    // PLAYER
    auto player = std::make_unique<dae::GameObject>();
    player->AddComponent<dae::SpriteComponent>("PlayerSprite.png", 3, 3, 0.1f);
    player->SetLocalPosition(650, 400);

    auto playerPtr = player.get();
    scene.Add(std::move(player));

    // INPUT BINDINGS
    auto& input = dae::InputManager::GetInstance();
    float playerSpeed = 150.0f;

    // WASD
    input.BindCommand(SDL_SCANCODE_W, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, -1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_S, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, 1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_A, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ -1, 0 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_D, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 1, 0 }, playerSpeed));

    // Arrow Keys
    input.BindCommand(SDL_SCANCODE_UP, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, -1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_DOWN, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 0, 1 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_LEFT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ -1, 0 }, playerSpeed));
    input.BindCommand(SDL_SCANCODE_RIGHT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(playerPtr, glm::vec2{ 1, 0 }, playerSpeed));
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