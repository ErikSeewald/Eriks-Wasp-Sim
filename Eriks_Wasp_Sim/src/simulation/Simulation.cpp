/**
 * @file Simulation.cpp
 * @brief This file specifies the implementation of Simulation.h
 */

#include "Simulation.h"
#include <thread>
#include <DirectXMath.h>
#include <iostream>

using namespace std::chrono;
using DirectX::XMFLOAT3;

//WASPS
std::list<Wasp*> wasps;

//DELTA TIME
std::chrono::duration<double> deltaTime;
steady_clock::time_point previousTime;

/**
* Starts and runs the simulation loop.
*/
void Simulation::loop() {

    _loopInit();

    static const double secondsBetweenFrames = 1.0 / 60.0;

    while (true) 
    {

        for (Wasp* wasp : wasps)
        {
            wasp->update();
        }

        //DELTA TIME
        steady_clock::time_point currentTime = steady_clock::now();
        deltaTime = duration_cast<duration<double>>(currentTime - previousTime);
        previousTime = currentTime;

        double sleepTimeSeconds = secondsBetweenFrames - deltaTime.count();
        if (sleepTimeSeconds > 0)
        {
            std::this_thread::sleep_for(milliseconds(static_cast<int>(sleepTimeSeconds * 1000)));
        }
    }
}

/**
* Initializes everything needed for loop() to run
*/
void Simulation::_loopInit()
{
    //SETUP DELTA TIME
    previousTime = steady_clock::now();

    //SETUP WASPS
    for (int x = 0; x < 25; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            for (int z = 0; z < 25; z++)
            {
                Wasp* wasp = new Wasp();
                wasp->setPosition(XMFLOAT3(x * 2, y * 2, z * 2));
                wasps.push_back(wasp);
            }
        }
    }
}

/**
* Returns a list of all active wasps in the simulation.
*
* @return list of all active wasps in the simulation.
*/
std::list<Wasp*>* Simulation::getWasps()
{
    return &wasps;
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