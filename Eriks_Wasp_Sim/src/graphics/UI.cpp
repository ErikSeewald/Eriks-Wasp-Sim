#include "UI.h"
#include "SimVisualizer.h"
#include "WaspSlots.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glut.h"

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
    _drawHiveUI();

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
    const static ImVec2 initSize(350, 320);
    const static ImVec2 initPos(20,20);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Selected Wasp"))
    {
        // POSITION
        if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _drawVectorTable(wasp->getPosition(), "PositionTable");
        }

        // MOVEMENT
        if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Viewing vector: ");
            _drawVectorTable(wasp->getViewingVector(), "ViewingVectorTable");

            ImGui::Text("Turn speed: %.3f", wasp->getTurnSpeed());
            ImGui::Text("Ascend speed: %.3f", wasp->getAscendSpeed());
        }

        // HEALTH
        if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Use conditional coloring for Alive status
            bool isAlive = wasp->isAlive();
            ImGui::Text("Alive: ");
            ImGui::SameLine();
            ImGui::Text(isAlive ? "true" : "false");

            // HP BAR
            int hp = wasp->getHP();
            int maxHP = wasp->getMaxHP();
            ImGui::Text("HP: %d/%d", hp, maxHP);

            float healthPercentage = (float) hp / maxHP;
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(1) << healthPercentage * 100 << "%";

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 0.2f));
            ImGui::ProgressBar(healthPercentage, ImVec2(0.f, 0.f), stream.str().c_str()); 
            ImGui::PopStyleColor(1);
        }
    }

    ImGui::End();
}

void UI::_drawHiveUI()
{
    // Initial size and position
    const static ImVec2 initSize(150, 85);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 20, 20);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_FirstUseEver);

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