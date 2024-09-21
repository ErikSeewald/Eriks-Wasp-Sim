#include "eriks_wasp_sim.h"
#include "SimVisualizer.h"
#include "Simulation.h"
#include "Console.h"
#include "MemoryManager.h"
#include <thread>

int main(int argc, char** argv) 
{
    std::thread simThread(Simulation::startLoop);
    std::thread consoleThread(Console::startLoop);
    std::thread memoryThread(MemoryManager::startLoop);
    SimVisualizer::initGlut(argc, argv); 

    simThread.join();
    consoleThread.join();
    memoryThread.join();
    return 0;
}