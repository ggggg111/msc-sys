#include <iostream>

#include "imgui.h"
#include "IconsFontAwesome6.h"

#include "Visualizer.h"

#include "utils/Log.h"
#include "Application.h"
#include "utils/Loading.h"
#include "modules/Editor.h"
#include "modules/Renderer.h"
#include "modules/UserInterface.h"
#include "gui/Setup.h"

Visualizer::Visualizer() : GuiElement()
{

}

Visualizer::~Visualizer()
{

}

void Visualizer::Update()
{
	if(ImGui::Begin("Visualizer"))
	{
		if(ImGui::BeginTabBar("TabBarMode"))
		{
			if(ImGui::BeginTabItem("Video"))
			{
				if(App->editor->GetVideo())
				{
					std::vector<SDL_Texture*> stream = App->editor->GetVideoTextureStream();

					int video_num_frames = App->editor->setup->GetVideoNumFrames();
					int stream_num_frames = (int)stream.size();

					if(!stream.empty() && stream_num_frames == video_num_frames)
					{
						static int current_frame = 0;
						int num_frames = stream.size() - 1;

						static bool is_playing = false;
						static float play_speed = 1.0f;

						float original_w = App->editor->setup->GetVideoOriginalWidth();
						float original_h = App->editor->setup->GetVideoOriginalHeight();

						float aspect_ratio = original_w / original_h;
						float avail_x = ImGui::GetContentRegionAvail().x;

						ImGui::InputFloat("Speed", &play_speed, 0.01f, 1.0f, "%.1f");

						if(is_playing)
						{
							if(ImGui::Button(ICON_FA_PAUSE))
								is_playing = false;
						}
						else
						{
							if(ImGui::Button(ICON_FA_PLAY))
								is_playing = true;
						}

						ImGui::SameLine();

						if(ImGui::SliderInt("Frame", &current_frame, 0, num_frames))
							is_playing = false;

						if(is_playing)
						{
							current_frame = (int)(play_speed * ImGui::GetIO().DeltaTime * 50.0f + current_frame) % num_frames;
						}

						ImGui::Image((void*)stream[current_frame], ImVec2(avail_x, avail_x / aspect_ratio));
					}
					else if(!stream.empty() && stream_num_frames < video_num_frames)
					{
						float frames_progress_ratio = (float)stream_num_frames / (float)video_num_frames;

						ImGui::Text("Processing the video...");
						ImGui::SameLine();
						UserInterface::HelpMarker("It might take a while when visualizing the video with its corresponding bounding boxes and keypoints");

						float progress_saturated = IM_CLAMP(frames_progress_ratio, 0.0f, 1.0f);
						char buf[32];
						sprintf_s(buf, "%d/%d", (int)(progress_saturated * video_num_frames), video_num_frames);

						ImGui::ProgressBar(frames_progress_ratio, ImVec2(0.f, 0.f), buf);
						ImGui::SameLine();
						ImGui::Text("Frames");
					}
				}

				ImGui::EndTabItem();
			}

			if(ImGui::BeginTabItem("Skeleton"))
			{
				if(App->editor->GetSkeleton())
				{
					auto kpdict = App->editor->GetSkeleton()->copy();
					torch::Tensor keypoints = kpdict.at("keypoint").clone();

					int num_persons = keypoints.size(0);
					int num_frames = keypoints.size(1);
					int num_nodes = keypoints.size(2);

					auto pairs = num_nodes == COCO_NUM_KEYPOINTS ? App->editor->GetCocoPairs() : App->editor->GetNtuPairs();

					float viz_ratio = this->skeleton_visualization_ratio;

					if(num_nodes == NTURGBD_NUM_KEYPOINTS)
					{
						keypoints.index({ "...", 1 }) = -keypoints.index({ "...", 1 });

						keypoints.index({ "...", 0 }) /= keypoints.index({ "...", 2 });
						keypoints.index({ "...", 1 }) /= keypoints.index({ "...", 2 });

						viz_ratio = 1080.0f / 1920.0f;
					}

					torch::Tensor keypoints_x = keypoints.index({ "...", 0 }).flatten();
					torch::Tensor keypoints_y = keypoints.index({ "...", 1 }).flatten();

					torch::Tensor keypoints_x_nonzero = keypoints_x.index_select(0, torch::nonzero(keypoints_x).squeeze());
					torch::Tensor keypoints_y_nonzero = keypoints_y.index_select(0, torch::nonzero(keypoints_y).squeeze());

					float x_max_nonzero = keypoints_x_nonzero.max().item<float>();
					float x_min_nonzero = keypoints_x_nonzero.min().item<float>();
					float y_max_nonzero = keypoints_y_nonzero.max().item<float>();
					float y_min_nonzero = keypoints_y_nonzero.min().item<float>();

					torch::Tensor kx = keypoints.index({ "...", 0 });
					torch::Tensor ky = keypoints.index({ "...", 1 });

					keypoints.index({ "...", 0 }) = torch::where(kx > 0, kx - x_min_nonzero, kx);
					keypoints.index({ "...", 1 }) = torch::where(ky > 0, ky - y_min_nonzero, ky);

					float x_max = keypoints.index({ "...", 0 }).max().item<float>();
					float x_min = keypoints.index({ "...", 0 }).min().item<float>();
					float y_max = keypoints.index({ "...", 1 }).max().item<float>();
					float y_min = keypoints.index({ "...", 1 }).min().item<float>();

					keypoints.index({ "...", 0 }) = (keypoints.index({ "...", 0 }) - x_min) / (x_max - x_min);
					keypoints.index({ "...", 1 }) = (keypoints.index({ "...", 1 }) - y_min) / (y_max - y_min);

					static int current_frame = 0;
					static bool is_playing = false;
					static float play_speed = 1.0f;
					static bool show_keypoint_indices = true;

					const char* modes[] = { "Bones", "Joints" };
					ImGui::Combo("Mode", (int*)&this->visualization_mode, modes, IM_ARRAYSIZE(modes));

					ImGui::InputFloat("Speed", &play_speed, 0.01f, 1.0f, "%.1f");

					if(is_playing)
					{
						if(ImGui::Button(ICON_FA_PAUSE))
							is_playing = false;
					}
					else
					{
						if(ImGui::Button(ICON_FA_PLAY))
							is_playing = true;
					}

					ImGui::SameLine();

					if(ImGui::SliderInt("Frame", &current_frame, 0, num_frames - 1))
						is_playing = false;

					ImGui::Checkbox("Show indices", &show_keypoint_indices);

					if(is_playing)
					{
						current_frame = (int)(play_speed * ImGui::GetIO().DeltaTime * 50.0f + current_frame) % num_frames;
					}

					for(int p = 0; p < num_persons; ++p)
					{
						for(const auto& pair : pairs)
						{
							float avail_x = ImGui::GetContentRegionAvail().x;
							float avail_y = ImGui::GetContentRegionAvail().y;

							int pair_a = pair.first;
							int pair_b = pair.second;

							float x1 = keypoints[p][current_frame][pair_a][0].item<float>();
							float y1 = keypoints[p][current_frame][pair_a][1].item<float>();
							float x2 = keypoints[p][current_frame][pair_b][0].item<float>();
							float y2 = keypoints[p][current_frame][pair_b][1].item<float>();

							float viewport_ratio = avail_y / avail_x;

							ImVec2 point_a = ImGui::GetCursorScreenPos();
							point_a.x += x1 * avail_x * viz_ratio * viewport_ratio;
							point_a.y += y1 * avail_y;

							ImVec2 point_b = ImGui::GetCursorScreenPos();
							point_b.x += x2 * avail_x * viz_ratio * viewport_ratio;
							point_b.y += y2 * avail_y;

							switch(this->visualization_mode)
							{
								case BONES:
								{
									ImGui::GetWindowDrawList()->AddLine(point_a, point_b, ImGui::GetColorU32(ImGuiCol_Text), 3.0f);

									break;
								}

								case JOINTS:
								{
									ImGui::GetWindowDrawList()->AddCircleFilled(point_a, 6.0f, ImGui::GetColorU32(ImGuiCol_Text));
									ImGui::GetWindowDrawList()->AddCircleFilled(point_b, 6.0f, ImGui::GetColorU32(ImGuiCol_Text));

									break;
								}

								default:
									break;
							}

							if(show_keypoint_indices)
							{
								float offset = ImGui::GetFontSize() * 0.5f;

								ImVec2 text_pos_a = ImVec2(point_a.x - offset, point_a.y - offset);
								ImGui::GetWindowDrawList()->AddText(text_pos_a, ImGui::GetColorU32(ImGuiCol_ButtonHovered), std::to_string(pair_a).c_str());
								//ImGui::GetWindowDrawList()->AddText(text_pos_a, ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), std::to_string(pair_a).c_str());

								ImVec2 text_pos_b = ImVec2(point_b.x - offset, point_b.y - offset);
								ImGui::GetWindowDrawList()->AddText(text_pos_b, ImGui::GetColorU32(ImGuiCol_ButtonHovered), std::to_string(pair_b).c_str());
								//ImGui::GetWindowDrawList()->AddText(text_pos_b, ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), std::to_string(pair_b).c_str());
							}
						}
					}
				}

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Visualizer::SetSkeletonRatio(const float& ratio)
{
	this->skeleton_visualization_ratio = ratio;
}