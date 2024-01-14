#include "eriks_wasp_sim.h"
#include "SimVisualizer.h"
#include "Simulation.h"
#include <thread>

int main(int argc, char** argv) {
    std::thread simThread(Simulation::loop); //Simulation thread

    SimVisualizer::initGlut(argc, argv); // Visualization GLUT loop

    simThread.join();
    return 0;
}