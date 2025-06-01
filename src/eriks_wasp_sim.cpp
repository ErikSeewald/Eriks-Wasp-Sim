#include "eriks_wasp_sim.h"
#include "SimVisualizer.h"
#include "Simulation.h"
#include "DirectoryHandler.h"
#include "Console.h"
#include <thread>

int main(int argc, char** argv) 
{
    DirectoryHandler::init();

    std::thread simThread(Simulation::startLoop);
    std::thread consoleThread(Console::startLoop);
    SimVisualizer::init(argc, argv); 

    simThread.join();
    consoleThread.join();
    return 0;
}
