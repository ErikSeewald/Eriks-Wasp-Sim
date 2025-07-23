#include "Simulation.h"
#include "UI.h"
#include "WaspSlots.h"
#include "ResourceSpawner.h"
#include "ThreadPool.h"
#include <thread>
#include <mutex>
#include <iostream>

using namespace std::chrono;
using Strategies::SpawnStrategy;
using Food::FoodEntity;

//DELTA TIME
std::chrono::duration<double> deltaTime;
steady_clock::time_point previousTime;

//THREADS
static const int threadPoolSize = ThreadPool::choosePoolSize();
static ThreadPool sim_pool(threadPoolSize);

/**
* Starts and runs the simulation loop.
*/
void Simulation::startLoop() 
{
    _loopInit();

    static const double secondsBetweenUpdates = 1.0 / 60.0;
    while (true) 
    {
        // SIMULATION LOGIC
        if (!UI::getUIState()->isPaused) 
        {
            updateWasps();
            ResourceSpawner::update(&deltaTime);
        }

        // MAX INDICES
        static const double secondsBetweenMaxIndexUpdates = 5.0;
        static double secondsSinceMaxIndexUpdate = 0.0;

        secondsSinceMaxIndexUpdate += deltaTime.count();
        if (secondsSinceMaxIndexUpdate > secondsBetweenMaxIndexUpdates)
        {
            WaspSlots::updateMaxIndex();
            Food::updateMaxIndex();
            secondsSinceMaxIndexUpdate = 0.0;
        }

        // DELTA TIME
        updateDeltaTime();

        double sleepTimeSeconds = secondsBetweenUpdates - deltaTime.count();
        if (sleepTimeSeconds > 0) {
            std::this_thread::sleep_for(milliseconds(static_cast<int>(sleepTimeSeconds * 1000)));
        }
    }

}

void Simulation::updateWasps()
{
    //WASPS (Divide wasp array into local sections and use threads to update them)
    std::vector<Wasp>* wasps = WaspSlots::getWasps();

    int maxIndex = WaspSlots::getMaxIndex();
    int sectionSize = std::floor(maxIndex / threadPoolSize);

    for (int i = 0; i < threadPoolSize; ++i)
    {
        int start = sectionSize * i;
        int end = (i < threadPoolSize - 1) ? sectionSize * (i + 1) : maxIndex;

        sim_pool.enqueue([start, end, &wasps] 
        {
            for (int j = start; j < end; ++j)
            {
                Wasp* wasp = &(*wasps)[j];
                wasp->update();
            }
        });
    }
    sim_pool.waitFinishAll();

    //SELECTED WASP
    UI::UI_STATE* uiState = UI::getUIState();
    Wasp* selectedWasp = uiState->selectedWasp;
    if (selectedWasp != nullptr && !selectedWasp->isAlive)
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
    WaspSlots::init();

    static const int initWaspCount = 250;
    WaspSlots::spawnWasps(glm::vec3(5, 5, 5), initWaspCount, SpawnStrategy::RANDOM, 10);
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

FoodEntity* Simulation::getFirstFoodInApproxRadius(glm::vec3 position, float radius)
{
    // TODO: Currently horribly inefficient. Fix this

    // Bounding box for radius approximation
    glm::vec3 minBound = position - glm::vec3(radius);
    glm::vec3 maxBound = position + glm::vec3(radius);

    std::vector<FoodEntity>* foodEntities = Food::getFoodEntities();
    int maxIndex = Food::getMaxIndex();
    for (int i = 0; i < maxIndex; ++i)
    {
        FoodEntity* food = &(*foodEntities)[i];
        if (food->eaten) { continue; }

        // Is the food within the bounding box
        if (food->position.x >= minBound.x && food->position.x <= maxBound.x &&
            food->position.y >= minBound.y && food->position.y <= maxBound.y &&
            food->position.z >= minBound.z && food->position.z <= maxBound.z)
        {
            return food;
        }
    }

    return nullptr;
}


/**
* Synchronous function that locks the food mutex and checks whether the given FoodEntity has been eaten.
* If so, it returns false. Otherwise it sets food->eaten to true and returns true.
*/
bool Simulation::attemptEatFoodMutex(FoodEntity* food)
{
    // TODO: Currently only one mutex for all food. Not sure wether the trade of for
    // individual mutexes for individual food entites would be worth it.
    static std::mutex foodMutex;

    std::lock_guard<std::mutex> lock(foodMutex);
    if (food->eaten)
    { 
        return false; 
    }

    food->eaten = true;
    Food::registerEntityEaten();
    return true;
}