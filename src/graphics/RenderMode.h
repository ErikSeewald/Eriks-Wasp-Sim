#pragma once

/**
* @namespace RenderMode
* @brief A namespace for enums containing various rendering modes of the WaspSim. 
*/
namespace RenderMode
{
    /**
    * @enum WaspRenderMode
    * @brief An enum of different render modes for wasps. Used to signal the wasp shader how to draw the wasp entities.
    */
    enum class WaspRenderMode
    {
        UniformFlat,
        UniformColor,
        RandomOranges,
        FullRandomColors,
        GreenIfHasGoal,
        RelativeWorkerScore,
        RelativeHunger,
        RelativeHealth,
        IsContractPartner,
    };
}