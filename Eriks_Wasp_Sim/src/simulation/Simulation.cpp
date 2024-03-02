#include "Simulation.h"
#include "UI.h"
#include "WaspSlots.h"
#include "MemoryManager.h"
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
    spawnWasps(glm::vec3(5, 5, 5), initWaspCount, SpawnStrategy::RANDOM);
}

using Simulation::SpawnStrategy;
bool Simulation::spawnWasps(glm::vec3 position, int amount, SpawnStrategy strategy)
{
    if (strategy == SpawnStrategy::RANDOM)
    {
        double spawnRadius = std::pow(3*amount / 4* 3.14159, 1 / 3.);

        for (int i = 0; i < amount; i++)
        {
            Wasp* wasp = new Wasp();

            double x = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double y = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double z = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            wasp->setPosition(glm::vec3(position.x + x, position.y + y, position.z + z));

            WaspSlots::allocateWaspSlot(wasp);
        }
    }

    else
    {
        for (int i = 0; i < amount; i++)
        {
            Wasp* wasp = new Wasp();
            wasp->setPosition(position);
            WaspSlots::allocateWaspSlot(wasp);
        }
    }
    
    return amount >= 1;
}

using Simulation::KillStrategy;
int Simulation::killWasps(int amountToKill, KillStrategy strategy)
{
    if (strategy == KillStrategy::ALL)
    {
        amountToKill = -1;
    }

    int killedAmount = 0;
    WaspSlot* waspSlot = WaspSlots::getWaspSlots();
    while (waspSlot != nullptr && killedAmount != amountToKill)
    {
        waspSlot->wasp->kill();
        killedAmount++;
        waspSlot = waspSlot->next;
    }

    MemoryManager::scheduleCleanup();
    return killedAmount;
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