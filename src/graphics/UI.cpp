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
    _drawContractUI();
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
    const static ImVec2 initSize(300, 700);
    const static ImVec2 initPos(10,10);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Selected Wasp"))
    {
        int w_Index = wasp->w_Index;
        ImGui::Text("w_Index: %i", w_Index);
        ImGui::Text("workerScore: %i", WaspSlots::getQueen().getWorkerScore(w_Index));

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
            int maxHP = wasp->balancedGenes.maxHP;
            ImGui::Text("HP: %d/%d", hp, maxHP);

            _drawPercentageBar(hp, maxHP, ImVec4(0.0f, 1.0f, 0.0f, 0.2f));
        }

        // HUNGER
        if (ImGui::CollapsingHeader("Hunger", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // HUNGER BAR
            int hungerSaturation = wasp->hungerSaturation;
            int maxSaturation = wasp->balancedGenes.maxHungerSaturation;
            ImGui::Text("Hunger saturation: %d/%d", hungerSaturation, maxSaturation);

            _drawPercentageBar(hungerSaturation, maxSaturation, ImVec4(1.0f, 0.3f, 0.0f, 0.3f));
        }

        // GENES
        if (ImGui::CollapsingHeader("Genes"))
        {
            if (ImGui::CollapsingHeader("Unbound Genes"))
            {
                if (ImGui::BeginTable("Unbound Genes", 2, ImGuiTableFlags_Borders))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Queen loyalty");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", wasp->unboundGenes.queenLoyalty);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Contract desire");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", wasp->unboundGenes.contractDesire);
                    ImGui::EndTable();
                }
            }

            if (ImGui::CollapsingHeader("Balanced Genes"))
            {
                if (ImGui::BeginTable("Balanced Genes", 2, ImGuiTableFlags_Borders))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Max HP");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", wasp->balancedGenes.maxHP);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Max Hunger Sat.");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", wasp->balancedGenes.maxHungerSaturation);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Flying speed");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", wasp->balancedGenes.flyingSpeed);
                    ImGui::EndTable();
                }
            }
        }

        // CONTRACTS
        if (ImGui::CollapsingHeader("Contracts", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("Contracts", 2, ImGuiTableFlags_Borders))
            {
                ImGui::TableHeadersRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Contract type");
                ImGui::TableSetColumnIndex(1); ImGui::Text("Valid for (s)");

                for (int i = 0; i < Wasp::MAX_NUM_CONTRACTS; i++)
                {
                    Contracts::Contract* contract = wasp->contracts[i];
                    if (contract == nullptr) { continue; }
                    
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    bool isSelected = (uiState.selectedContract == contract);
                    if (ImGui::Selectable(Contracts::contractTypeAsStr(contract->getType()),
                            isSelected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        uiState.selectedContract = contract;
                    }

                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", contract->getRemainingValiditySeconds());
                }

                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}

void UI::_drawContractUI()
{
    Contracts::Contract* contract = uiState.selectedContract;
    if (contract == nullptr)
    {
        return;
    }
    const std::vector<Wasp*>& partners = contract->getPartners();

    // Initial size and position
    const static ImVec2 initSize(300, 300);
    const static ImVec2 initPos(320,10);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Contract"))
    {
        if (ImGui::CollapsingHeader("General information", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Valid for (s): %.1f", contract->getRemainingValiditySeconds());

            // Selectable list of partners
            ImGui::Separator();
            ImGui::Text("Partners (%zu)", partners.size());
            if (ImGui::BeginChild("PartnersList", ImVec2(0.0f, 100.0f), true))
            {
                for (Wasp* partner : partners)
                {
                    ImGui::Bullet();
                    ImGui::SameLine();

                    std::string label = "Wasp " + std::to_string(partner->w_Index);
                    if (ImGui::Selectable(label.c_str()))
                    {
                        SimVisualizer::jumpToAndLookAt(partner->position);
                        uiState.selectedWasp = partner;
                    }
                }
            }
            ImGui::EndChild();
        }

        // TODO: Even better contract UI
        Contracts::ContractType type = contract->getType();
        if (ImGui::CollapsingHeader(Contracts::contractTypeAsStr(type), ImGuiTreeNodeFlags_DefaultOpen))
        {
            switch (type)
            {
                case Contracts::ContractType::FoodSharingContractType:
                {
                    Contracts::FoodSharingContract* fsc = (Contracts::FoodSharingContract*) contract;
                    if (ImGui::BeginTable("FoodSharingContract", 2, ImGuiTableFlags_Borders))
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("hungerAllowance");
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%i", fsc->hungerSaturationAllowance);
                    
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("sharingRate");
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", fsc->sharingRate);

                        ImGui::EndTable();
                    }
                    break;
                }

                case Contracts::ContractType::CliqueContractType:
                {
                    Contracts::CliqueContract* cc = (Contracts::CliqueContract*) contract;

                    if (ImGui::BeginTable("CliqueContract", 2, ImGuiTableFlags_Borders))
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Partner 1");
                        ImGui::TableSetColumnIndex(1); ImGui::Text("Wasp %i", partners.at(0)->w_Index);
                    
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Range");
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", cc->range);

                        ImGui::EndTable();
                    }
                    break;
                }
            }
        }
    }

    ImGui::End();
}

void UI::_drawPerformanceUI()
{
    static float fps = 0.0f;
    static int lastUpdateTime = 0;

    const static ImVec2 initSize(150, 85);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 10, 10);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Performance"))
    {
        // only sample framerate every 500 ms
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
    const static ImVec2 initSize(150, 150);
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

        ImGui::Separator();
        ImGui::Text("QueenFoodStorage:\n%d", WaspSlots::getQueen().getStoredFoodAmount());

        ImGui::Separator();
        ImGui::Text("Contracts: \n%d", Contracts::getNumActiveContracts());
    }

    ImGui::End();
}


void UI::_drawCameraUI()
{
    const static ImVec2 initSize(150, 190);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 10, 265);
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
    const static ImVec2 initSize(150, 150);
    const static ImVec2 initPos(glutGet(GLUT_WINDOW_WIDTH) - initSize.x - 10, 465);
    ImGui::SetNextWindowPos(initPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(initSize, ImGuiCond_Once);

    if (ImGui::Begin("Options"))
    {
        ImGui::Checkbox("Pause sim", &uiState.isPaused);
        ImGui::Checkbox("Draw grid", &uiState.drawGrid);
        ImGui::Separator();

        // WaspRenderMode
        ImGui::Text("WaspRenderMode:");
        const int MODES = 9;
        const static char* WASP_RENDER_MODE_STRING_NAMES[MODES] = 
        {
            "UniformFlat", "UniformColor", "RandomOranges", "FullRandomColors", 
            "GreenIfHasGoal", "RelativeWorkerScore", "RelativeHunger", "RelativeHealth",
            "IsContractPartner"
        };
        int current = static_cast<int>(uiState.waspRenderMode);
        bool changed = ImGui::Combo(" ", &current, WASP_RENDER_MODE_STRING_NAMES, MODES);
        if (changed) { uiState.waspRenderMode = static_cast<RenderMode::WaspRenderMode>(current); }

        ImGui::Separator();
    }

    ImGui::End();
}

void UI::_drawVectorTable(const glm::vec3& vector, const std::string& tableName)
{
    if (ImGui::BeginTable(tableName.c_str(), 2, ImGuiTableFlags_Borders))
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