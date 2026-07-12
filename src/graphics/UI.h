#pragma once

#include "Wasp.h"
#include "Contracts.h"
#include "RenderMode.h"
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glut.h"

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
		bool drawSelectedWaspGoal = false;
		bool drawGrid = true;
		bool isPaused = false;
		RenderMode::WaspRenderMode waspRenderMode = RenderMode::WaspRenderMode::RandomOranges;
		Contracts::Contract* selectedContract;
	};

	UI_STATE* getUIState();

	void drawUI();

	void _drawSelectedWaspUI();

	void _drawContractUI();

	void _drawPerformanceUI();

	void _drawHiveUI();

	void _drawCameraUI();

	void _drawOptionsUI();

	void _drawVectorTable(const glm::vec3& vector, const std::string& tableName);

	void _drawPercentageBar(int value, int maxValue, ImVec4 color);
};