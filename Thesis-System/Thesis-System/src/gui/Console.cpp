#include <iostream>
#include <list>

#include "imgui.h"

#include "Console.h"

std::list<std::string> Logs = {};

Console::Console() : GuiElement(), auto_scroll(true)
{

}

Console::~Console()
{

}

void Console::Update()
{
	if(ImGui::Begin("Console"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		float old_font_size = ImGui::GetFont()->Scale;
		ImGui::GetFont()->Scale *= 0.8f;
		ImGui::PushFont(ImGui::GetFont());

		for(auto item = Logs.begin(); item != Logs.end(); ++item)
		{
			ImGui::TextWrapped(item->c_str());
		}

		ImGui::GetFont()->Scale = old_font_size;
		ImGui::PopFont();

		ImGui::PopStyleVar();

		if(this->auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::End();
	}
}

void Console::CleanUp()
{
	Logs.clear();
}