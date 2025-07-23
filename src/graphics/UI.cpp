#include "UI.h"
#include "SimVisualizer.h"
#include "WaspSlots.h"
#include <sstream>
#include <iomanip>

using UI::UI_STATE;

UI_STATE uiState;

UI_STATE* UI::getUIState()
{
	return &uiState;
}

void UI::drawUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    _drawSelectedWaspUI();
    _drawPerformanceUI();
    _drawHiveUI();
    _drawCameraUI();
    _drawOptionsUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::_drawSelectedWaspUI()
{
    Wasp* wasp = uiState.selectedWasp;
    if (wasp == nullptr)
    {
        return;
    }

    // Initial size and position
    const static ImVec2 initSize(250, 420);
    const static ImVec2 initPos(10,10);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Selected Wasp"))
    {
        ImGui::Text("w_Index: %i", wasp->w_Index);

        // POSITION
        if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _drawVectorTable(wasp->position, "WaspPositionTable");
        }

        // MOVEMENT
        if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Viewing vector: ");
            _drawVectorTable(wasp->viewingVector, "WaspViewingVectorTable");

            ImGui::Text("Turn speed: %.3f", wasp->turnSpeed);
            ImGui::Text("Ascend speed: %.3f", wasp->ascendSpeed);

            if (ImGui::CollapsingHeader("Current goal", ImGuiTreeNodeFlags_None))
            {
                if (uiState.selectedWasp->currentGoal != nullptr)
                {
                    _drawVectorTable(*uiState.selectedWasp->currentGoal, "WaspGoalTable");
                }
                
                else
                {
                    ImGui::Text("No current goal");
                }
                ImGui::Checkbox("Display current goal", &uiState.drawSelectedWaspGoal);
            }
            
        }

        // HEALTH
        if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool isAlive = wasp->isAlive;
            ImGui::Text("Alive: ");
            ImGui::SameLine();
            ImGui::Text(isAlive ? "true" : "false");

            // HP BAR
            int hp = wasp->hp;
            int maxHP = wasp->MAX_HP;
            ImGui::Text("HP: %d/%d", hp, maxHP);

            _drawPercentageBar(hp, maxHP, ImVec4(0.0f, 1.0f, 0.0f, 0.2f));
        }

        // HUNGER
        if (ImGui::CollapsingHeader("Hunger", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // HUNGER BAR
            int hungerSaturation = wasp->hungerSaturation;
            int maxSaturation = wasp->MAX_HUNGER_SATURATION;
            ImGui::Text("Hunger saturation: %d/%d", hungerSaturation, maxSaturation);

            _drawPercentageBar(hungerSaturation, maxSaturation, ImVec4(1.0f, 0.3f, 0.0f, 0.3f));
        }
    }

    ImGui::End();
}

void UI::_drawPerformanceUI()
{
    static float fps = 0.0f;
    static int lastUpdateTime = 0;

    // Initial size and position
    const static ImVec2 initSize(150, 85);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 10, 10);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Performance"))
    {
        // only sample framerate/frametime every 500 ms
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        if (currentTime - lastUpdateTime > 500)
        {
            fps = ImGui::GetIO().Framerate;
            lastUpdateTime = currentTime;
        }

        ImGui::Text("FPS: %.f", fps);
        ImGui::Text("Max w_index: %i", WaspSlots::getMaxIndex());
        ImGui::Text("Max f_index: %i", Food::getMaxIndex());
    }

    ImGui::End();
}

void UI::_drawHiveUI()
{
    // Initial size and position
    const static ImVec2 initSize(150, 85);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 10, 105);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Hive"))
    {
        int aliveCount = WaspSlots::getAliveCount();
        int deadCount = WaspSlots::getDeadCount();
        ImGui::Text("Alive: %d", aliveCount);
        ImGui::Text("Dead: %d", deadCount);
        ImGui::Text("Total: %d", aliveCount + deadCount);
    }

    ImGui::End();
}


void UI::_drawCameraUI()
{
    // Initial size and position
    const static ImVec2 initSize(150, 190);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 10, 200);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Camera"))
    {
        Camera camera = SimVisualizer::getCamera();

        if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _drawVectorTable(camera.position, "CameraPositionTable");
        }

        if (ImGui::CollapsingHeader("Viewing Vector", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _drawVectorTable(camera.direction, "CameraViewingVectorTable");
        }
    }

    ImGui::End();
}

void UI::_drawOptionsUI()
{
    // Initial size and position
    const static ImVec2 initSize(150, 80);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 10, 400);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Options"))
    {
        ImGui::Checkbox("Draw grid", &uiState.drawGrid);
    }

    ImGui::End();
}

void UI::_drawVectorTable(const glm::vec3& vector, const std::string& tableName)
{
    if (ImGui::BeginTable("VectorTable", 2, ImGuiTableFlags_Borders))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("X");
        ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", vector.x);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Y");
        ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", vector.y);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Z");
        ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", vector.z);
        ImGui::EndTable();
    }
}

void UI::_drawPercentageBar(int value, int maxValue, ImVec4 color)
{
    float percentage = (float) value / maxValue;
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1) << percentage * 100 << "%";

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
    ImGui::ProgressBar(percentage, ImVec2(-5, 16), stream.str().c_str());
    ImGui::PopStyleColor(1);
}