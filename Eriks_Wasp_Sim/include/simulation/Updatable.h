/**
* @class Updatable
* @brief An abstract class meant to serve as an interface for updatable classes within the simulation.
*/
class Updatable 
{
    public:

        /**
        * Updates the state of the object.
        */
        virtual void update() = 0;
};