/**
 * @file Simulation.cpp
 * @brief This file specifies the implementation of Simulation.h
 */

#include "Simulation.h"
#include <thread>
#include <DirectXMath.h>
#include <iostream>

using DirectX::XMFLOAT3;

std::list<Wasp*> wasps;

/**
* Starts and runs the simulation loop.
*/
void Simulation::loop() {

    for (int x = 0; x < 20; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            for (int z = 0; z < 25; z++)
            {
                Wasp* wasp = new Wasp();
                wasp->setPosition(XMFLOAT3(x*2, y*2, z*2));
                wasps.push_back(wasp);
            }
        }
    }

    while (true) {
        
        for (Wasp* wasp : wasps)
        {
            wasp->update();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }
}

/**
* Returns a list of all active wasps in the simulation.
*
* @return list of all active wasps in the simulation.
*/
std::list<Wasp*> Simulation::getWasps()
{
    return wasps;
}