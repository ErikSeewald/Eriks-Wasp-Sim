#include "Simulation.h"
#include "UI.h"
#include "WaspSlots.h"
#include <thread>

#include <iostream>

using namespace std::chrono;
using EntitySlots::EntitySlot;
using Strategies::SpawnStrategy;


//#include "food.h"
using Food::FoodEntity;


//DELTA TIME
std::chrono::duration<double> deltaTime;
steady_clock::time_point previousTime;

/**
* Starts and runs the simulation loop.
*/
void Simulation::startLoop() {

    _loopInit();

    static const double secondsBetweenUpdates = 1.0 / 60.0;
    while (true) 
    {
        if (!UI::getUIState()->isPaused) 
        {
            updateWasps();
            updateFood();
        }

        updateDeltaTime();

        double sleepTimeSeconds = secondsBetweenUpdates - deltaTime.count();
        if (sleepTimeSeconds > 0) {
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

void Simulation::updateFood()
{
    FoodEntity* food;
    EntitySlot* nextSlot;
    EntitySlot* foodSlot = Food::getFoodSlots();

    while (foodSlot != nullptr)
    {
        food = (FoodEntity*)foodSlot->entity;
        nextSlot = foodSlot->next;

        if (food->eaten)
        {
            Food::removeFoodSlot(foodSlot);
        }

        foodSlot = nextSlot;
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

FoodEntity* Simulation::getFirstFoodInApproxRadius(glm::vec3 position, float radius)
{
    // Bounding box for radius approximation
    glm::vec3 minBound = position - glm::vec3(radius);
    glm::vec3 maxBound = position + glm::vec3(radius);

    EntitySlot* foodSlot = Food::getFoodSlots();
    while (foodSlot != nullptr)
    {
        FoodEntity* food = (FoodEntity*)foodSlot->entity;

        // Is the food within the bounding box
        if (food->position.x >= minBound.x && food->position.x <= maxBound.x &&
            food->position.y >= minBound.y && food->position.y <= maxBound.y &&
            food->position.z >= minBound.z && food->position.z <= maxBound.z)
        {
            if (!food->eaten)
            {
                return food;
            }
        }

        foodSlot = foodSlot->next;
    }

    return nullptr;
}