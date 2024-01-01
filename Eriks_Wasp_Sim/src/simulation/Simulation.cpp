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

    for (int i = 0; i < 100; i++)
    {
        Wasp* wasp = new Wasp();
        wasp->setPosition(XMFLOAT3(0.0f, 1.0f, 1.0f));
        wasps.push_back(wasp);
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