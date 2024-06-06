#include "imgui.h"
#include "IconsFontAwesome6.h"

#include "Application.h"
#include "MainMenuBar.h"
#include "utils/Log.h"

MainMenuBar::MainMenuBar() : GuiElement()
{

}

MainMenuBar::~MainMenuBar()
{

}

void MainMenuBar::Update()
{
	if(ImGui::BeginMainMenuBar())
	{
		ImGuiStyle& style = ImGui::GetStyle();

		const char* text1 = "File";
		const char* text2 = ICON_FA_SQUARE_ENVELOPE" Report a bug";

		float text1_width = ImGui::CalcTextSize(text1).x + style.FramePadding.x * 2.0f;
		float text2_width = ImGui::CalcTextSize(text2).x + style.FramePadding.x * 2.f;

		if(ImGui::BeginMenu(text1))
		{
			if(ImGui::MenuItem("Exit", "Esc"))
			{
				App->Exit();
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine((ImGui::GetWindowWidth() - text2_width - 10.0f));
		if(ImGui::Button(text2))
		{
			App->RequestBrowser("mailto:mfgerard21@m.fudan.edu.cn");
		}

		ImGui::EndMainMenuBar();
	}
}