#include "UI.h"

using UI::UI_STATE;

UI_STATE uiState;

UI_STATE* UI::getUIState()
{
	return &uiState;
}
