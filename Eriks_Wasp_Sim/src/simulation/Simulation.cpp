#include "Simulation.h"
#include "UI.h"
#include "WaspSlots.h"
#include "Food.h"
#include <thread>

using namespace std::chrono;
using EntitySlots::EntitySlot;
using Simulation::SpawnStrategy;
using Food::FoodEntity;

//DELTA TIME
std::chrono::duration<double> deltaTime;
steady_clock::time_point previousTime;

/**
* Starts and runs the simulation loop.
*/
void Simulation::startLoop() {

    _loopInit();

    while (true) 
    {
        if (!UI::getUIState()->isPaused)
        {
            updateWasps();
        }

        updateDeltaTime();

        static const double secondsBetweenUpdates = 1.0 / 60.0;
        double sleepTimeSeconds = secondsBetweenUpdates - deltaTime.count();
        if (sleepTimeSeconds > 0)
        {
            std::this_thread::sleep_for(milliseconds(static_cast<int>(sleepTimeSeconds * 1000)));
        }
    }
}

void Simulation::updateWasps()
{
    //WASPS
    Wasp* wasp;
    EntitySlot* nextSlot;
    EntitySlot* waspSlot = WaspSlots::getWaspSlots();

    while (waspSlot != nullptr)
    {
        wasp = (Wasp*) waspSlot->entity;
        nextSlot = waspSlot->next; // set nextSlot here incase 'waspSlot' gets removed

        if (wasp->isAlive())
        {
            wasp->update();
        }

        else
        {
            WaspSlots::removeWaspSlot(waspSlot);
        }

        waspSlot = nextSlot;
    }

    //SELECTED WASP
    UI::UI_STATE* uiState = UI::getUIState();
    Wasp* selectedWasp = uiState->selectedWasp;
    if (selectedWasp != nullptr && !selectedWasp->isAlive())
    {
        // deselect selectedWasp if it has died
        uiState->selectedWasp = nullptr;
    }
}

void Simulation::updateDeltaTime()
{
    steady_clock::time_point currentTime = steady_clock::now();
    deltaTime = duration_cast<duration<double>>(currentTime - previousTime);
    previousTime = currentTime;
}

/**
* Initializes everything needed for loop() to run
*/
void Simulation::_loopInit()
{
    //SETUP DELTA TIME
    previousTime = steady_clock::now();

    //SETUP WASPS
    static const int initWaspCount = 250;
    WaspSlots::spawnWasps(glm::vec3(5, 5, 5), initWaspCount, SpawnStrategy::RANDOM, 10);

    //SETUP FOOD
    FoodEntity* food = new FoodEntity();
    food->hungerPoints = 20;
    food->position = glm::vec3(4, 3, 2);
    Food::allocateFoodSlot(food);
}

/**
* Returns the simulations's last delta time
*
* @return the simulation's last delta time
*/
std::chrono::duration<double>* Simulation::getDeltaTime()
{
    return &deltaTime;
}