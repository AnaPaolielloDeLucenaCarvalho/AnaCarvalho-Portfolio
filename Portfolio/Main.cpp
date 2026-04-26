#define SDL_MAIN_HANDLED 

#include "Minigin.h"
#include <iostream>

void load()
{
    std::cout << "Welcome to the Portfolio!\n";
}

int main(int, char* [])
{
    std::cout << "Starting Portfolio Engine...\n";

    // Start the engine
    dae::Minigin engine("./Data/");
    engine.Run(load);

    return 0;
}