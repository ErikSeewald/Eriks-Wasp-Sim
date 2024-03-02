#include "MemoryManager.h"
#include "Simulation.h"
#include "WaspSlots.h"
#include <thread>
#include <chrono>

const std::chrono::duration<double> secondsBetweenLoops(0.1);
const int loopsBetweenCleanups = 20;
int loopsUntilNextCleanup = -1;

void MemoryManager::startLoop()
{
    while (true)
    {
        if (loopsUntilNextCleanup > 0)
        {
            loopsUntilNextCleanup--;
        }

        else if (loopsUntilNextCleanup == 0)
        {
            _memCleanup();
            loopsUntilNextCleanup = -1;
        }

        std::this_thread::sleep_for(secondsBetweenLoops);
    }
}

/**
* Allows other functions to schedule a memory cleanup. This will not happen immediately as to avoid
* concurrency conflicts with other threads that may still need a killed but not deleted entity 
* or something similar.
*/
void MemoryManager::scheduleCleanup()
{
    loopsUntilNextCleanup = loopsBetweenCleanups;
}

void MemoryManager::_memCleanup()
{
    WaspSlots::cleanupMemory();
}