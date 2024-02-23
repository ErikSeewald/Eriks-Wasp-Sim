#include "Simulation.h"
#include "UI.h"
#include "WaspSlots.h"
#include <thread>

using namespace std::chrono;
using WaspSlots::WaspSlot;

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
        updateWasps();

        updateDeltaTime();

        memoryCleanup();

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
    WaspSlot* nextSlot;
    WaspSlot* waspSlot = WaspSlots::getWaspSlots();

    while (waspSlot != nullptr)
    {
        wasp = waspSlot->wasp;
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
    if (selectedWasp != NULL && !selectedWasp->isAlive())
    {
        // deselect selectedWasp if it has died
        uiState->selectedWasp = NULL;
    }
}

void Simulation::updateDeltaTime()
{
    steady_clock::time_point currentTime = steady_clock::now();
    deltaTime = duration_cast<duration<double>>(currentTime - previousTime);
    previousTime = currentTime;
}

void Simulation::memoryCleanup()
{
    WaspSlots::cleanupMemory();
}

/**
* Initializes everything needed for loop() to run
*/
void Simulation::_loopInit()
{
    //SETUP DELTA TIME
    previousTime = steady_clock::now();

    //SETUP WASPS
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            for (int z = 0; z < 5; z++)
            {
                Wasp* wasp = new Wasp();
                wasp->setPosition(glm::vec3(x * 2, y * 2, z * 2));
                WaspSlots::allocateWaspSlot(wasp);
            }
        }
    }
}

bool Simulation::spawnWasps(glm::vec3 position, int amount)
{
    for (int i = 0; i < amount; i++)
    {
        Wasp* wasp = new Wasp();
        wasp->setPosition(position);
        WaspSlots::allocateWaspSlot(wasp);
    }
    
    return amount >= 1;
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