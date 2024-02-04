#pragma once

#include "Wasp.h"

/**
* @namespace UI
* @brief A namespace for the general user interface of the wasp sim
*/
namespace UI
{
	/**
	* @struct UI_STATE
	* @brief Struct to hold and pass around key values describing the current state of the user interface.
	*/
	struct UI_STATE
	{
		Wasp* selectedWasp;
	};

	UI_STATE* getUIState();
};