#include <iostream>
#include <filesystem>

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "utils/Log.h"
#include "Application.h"
#include "modules/Editor.h"
#include "Results.h"

Results::Results() : GuiElement()
{

}

Results::~Results()
{

}

void Results::Update()
{
	if(ImGui::Begin("Results"))
	{
		if(App->editor->GetOutput())
		{
			auto probabilities = App->editor->GetOutput();
			auto dataset_categories = App->editor->GetDatasetCategories();

			auto accessor = probabilities->accessor<float, 1>();

			for(int i = 0; i < dataset_categories.size(); ++i)
			{
				ImGui::PushID(i);

				ImGui::ProgressBar(
					accessor[i],
					ImVec2(
						0.3f * ImGui::GetContentRegionAvail().x,
						1.0f * ImGui::GetTextLineHeightWithSpacing()
					)
				);
				ImGui::SameLine();
				ImGui::TextUnformatted(dataset_categories[i].c_str());

				ImGui::PopID();
			}
		}

		ImGui::End();
	}
}