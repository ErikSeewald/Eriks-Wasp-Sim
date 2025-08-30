#pragma once

/**
* @class Updatable
* @brief An abstract class meant to serve as an interface for updatable classes within the simulation.
*/
class Updatable 
{
    public:
        virtual void update() = 0;
};