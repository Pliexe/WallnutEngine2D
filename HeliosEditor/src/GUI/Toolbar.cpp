#include "Toolbar.h"

#include "Application.h"
#include "pch.h"

#include "imgui.h"
#include "ProjectManager.h"
//#include "Panels/InspectorPanel.h"

namespace Helios
{
	void DrawToolbar(EditorMode mode, std::vector<Editor::IPanel*>& panels)
	{
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "", false, mode == EditorMode::Editor))
				{
					InspectorPanel::Reset();
					Project::LoadSampleScene();
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl+O", false, mode == EditorMode::Editor))
				{
					InspectorPanel::Reset();
					Project::OpenSceneDialog();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "Ctrl+S", false, mode == EditorMode::Editor))
					Project::SaveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, mode == EditorMode::Editor))
					Project::SaveScene(true);

				ImGui::Separator();

				if (ImGui::MenuItem("New Project"))
				{
					ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
				}

				if (ImGui::MenuItem("Open Project"))
				{
					ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) {
					Application::GetInstance().Quit();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				//if (ImGui::MenuItem("Profiler")) { show_profiler_window = true; }

				for (auto& panel : panels)
				{
					if (ImGui::MenuItem(panel->GetName().c_str())) { panel->m_window_open = true; }
				}

				if (ImGui::MenuItem("Scene View")) { panels.push_back(new ScenePanel()); }

				ImGui::EndMenu();
			}

			ImVec2 pos = ImGui::GetWindowPos();
			ImVec2 size = ImGui::GetWindowSize();

			ImGui::SetNextWindowPos(ImVec2(pos.x + size.x / 2.0f - 50.0f, pos.y));
			//ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::BeginChild("statePanel", ImVec2(58, 25));

			ImVec4 col = (mode == EditorMode::Playing ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			if (ImGui::ImageButton("playBtn", mode == EditorMode::Editor ? HeliosEditor::ICON_PLAY_WHITE->GetTextureID() : HeliosEditor::ICON_STOP_WHITE->GetTextureID(), ImVec2(25, 25), ImVec2(0, 0), ImVec2(1, 1), col))
			{
				switch (mode)
				{
				case EditorMode::Editor:
					mode = EditorMode::Playing;
					break;
				case EditorMode::Paused:
					mode = EditorMode::Editor;
					break;
				case EditorMode::Playing:
					mode = EditorMode::Editor;
					break;
				}
			}
			ImGui::SameLine();
			col = (mode == EditorMode::Paused ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			if (ImGui::ImageButton("pauseBtn", mode == EditorMode::Paused ? HeliosEditor::ICON_PLAY_WHITE->GetTextureID() : HeliosEditor::ICON_PAUSE_WHITE->GetTextureID(), ImVec2(25, 25), ImVec2(0, 0), ImVec2(1, 1), col))
			{
				switch (mode)
				{
				case EditorMode::Paused:
					mode = EditorMode::Playing;
					break;
				case EditorMode::Playing:
					mode = EditorMode::Paused;
					break;
				}
			}

			ImGui::EndChild();
			ImGui::PopStyleVar();
			//ImGui::PopStyleColor();

			//ImGui::ImageButton((ImTextureID)*ICON_PLAY_WHITE, ImVec2(20, 20));
			/*if ()
				mode = EditorMode::Playing;
			}*/

			ImGui::EndMenuBar();
		}
	}
}
