#pragma once

#include "Wasp.h"
#include <string>

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
		bool isPaused;
	};

	UI_STATE* getUIState();

	void drawUI();

	void _drawSelectedWaspUI();

	void _drawHiveUI();

	void _drawCameraUI();

	void _drawVectorTable(const glm::vec3& vector, const std::string& tableName);
};