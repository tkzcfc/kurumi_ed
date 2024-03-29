#include "Sequentity.h"
#include <algorithm>	// std::sort
#include "imgui_internal.h"
#include "cocos2d.h"

USING_NS_CC;

/*
----------------------
	IMPLEMENTATION
----------------------
*/

namespace Sequentity {

	/**
	 * @brief Handy math overloads, for readability
	 *
	 */
	inline ImVec4 operator*(const ImVec4& vec, const float mult) {
		return ImVec4{ vec.x * mult, vec.y * mult, vec.z * mult, vec.w };
	}

	inline ImVec2 operator+(const ImVec2& vec, const float value) {
		return ImVec2{ vec.x + value, vec.y + value };
	}

	//inline ImVec2 operator+(const ImVec2& vec, const ImVec2 value) {
	//	return ImVec2{ vec.x + value.x, vec.y + value.y };
	//}

	inline void operator-=(ImVec2& vec, const float value) {
		vec.x -= value;
		vec.y -= value;
	}

	inline ImVec2 operator-(const ImVec2& vec, const float value) {
		return ImVec2{ vec.x - value, vec.y - value };
	}

	//inline ImVec2 operator-(const ImVec2& vec, const ImVec2 value) {
	//	return ImVec2{ vec.x - value.x, vec.y - value.y };
	//}

	inline ImVec2 operator*(const ImVec2& vec, const float value) {
		return ImVec2{ vec.x * value, vec.y * value };
	}

	//inline ImVec2 operator*(const ImVec2& vec, const ImVec2 value) {
	//	return ImVec2{ vec.x * value.x, vec.y * value.y };
	//}

	static float global_scale = 1.0f;

	/**
	 * @brief A summary of all colours available to Sequentity
	 *
	 * Hint: These can be edited interactively via the `ThemeEditor()` window
	 *
	 */

	static struct CommonTheme_ {
		bool tint_track{ true };   // Visually distinguish a track by its color

		float transition_speed{ 0.2f };

		float track_height{ 25.0f };
		float event_height{ 20.0f };
		float channel_line_spacing{ 8.0f };
		float track_line_spacing{ 10.0f };

		float channel_head_padding{ 30.0f };
		float channel_tail_padding{ 30.0f };
		float event_radius{ 0.0f };
	} CommonTheme;


	static struct TimelineTheme_ {
		ImVec4 background{ ImColor::HSV(0.0f, 0.0f, 0.250f) };
		ImVec4 text{ ImColor::HSV(0.0f, 0.0f, 0.850f) };
		ImVec4 dark{ ImColor::HSV(0.0f, 0.0f, 0.322f) };
		ImVec4 mid{ ImColor::HSV(0.0f, 0.0f, 0.314f) };

		ImVec4 cursor_color{ ImColor::HSV(0.57f, 0.62f, 0.99f) };
		ImVec4 cursor_shadow{ ImColor::HSV(0.0f, 0.0f, 0.0f, 0.1f) };

		float height{ 40.0f };

	} TimelineTheme;


	static struct EditorTheme_ {
		ImVec4 background{ ImColor::HSV(0.0f, 0.0f, 0.188f) };
		ImVec4 left_panel_background{ ImColor::HSV(0.0f, 0.0f, 0.188f) };
		ImVec4 inactive_area_background{ ImColor::HSV(0.0f, 0.00f, 0.458f) };
		ImVec4 active_area_background{ ImColor::HSV(0.0f, 0.00f, 0.651f) };
		ImVec4 active_channel_background{ ImVec4(0.588f, 0.784f, 1.0f, 0.588f) };
		ImVec4 text{ ImColor::HSV(0.0f, 0.00f, 0.950f) };
		ImVec4 mid{ ImColor::HSV(0.0f, 0.00f, 0.600f) };
		ImVec4 dark{ ImColor::HSV(0.0f, 0.00f, 0.498f) };
		ImVec4 accent_dark{ ImColor::HSV(0.0f, 0.0f, 0.0f, 0.1f) };
		ImVec4 selection{ ImColor::HSV(0.0f, 0.0f, 1.0f) };
		ImVec4 outline{ ImColor::HSV(0.0f, 0.0f, 0.1f) };

		ImVec4 current_time{ ImColor::HSV(0.6f, 0.5f, 0.5f) };
	} EditorTheme;

	/**
	 * @brief Does the event intersect with this time?
	 *
	 */
	inline bool _contains(const Event& event, TimeType time) {
		return (
			event.time <= time &&
			event.time + event.length > time
			);
	}

	/**
	 * @brief Interpolate between current and target values
	 *
	 * ..at a given velocity (ignoring deltas smaller than epsilon)
	 *
	 */
	static void _transition(float& current, float target, float velocity, float epsilon = 0.1f) {
		const float delta = target - current;

		// Prevent transitions between too small values
		// (Especially damaging to fonts)
		if (abs(delta) < epsilon) current = target;

		current += delta * velocity;
	};


	void Intersect(const Track& track, TimeType time, IntersectFunc func) {
		for (auto& channel : track.channels) {
			if (track.mute) continue;
			if (track._notsoloed) continue;

			for (auto& event : channel.events) {
				if (!event.enabled) continue;
				if (_contains(event, time)) func(event);
			}
		}
	}

	void Intersect(const Track& track, TimeType time, IntersectWithPreviousFunc func) {
		for (auto& channel : track.channels) {
			if (track.mute) continue;
			if (track._notsoloed) continue;

			for (const auto& event : channel.events) {
				if (!event.enabled) continue;
				if (_contains(event, time)) {

					// Find closest
					// TODO: Find a better way to deal with this
					const Event* closest{ nullptr };
					for (auto& other : channel.events) {
						if (other.time < event.time) {
							if (closest == nullptr) {
								closest = &other;

							}
							else if (closest->time < other.time) {
								closest = &other;
							}
						}
					}

					func(closest, event);
				}
			}
		}
	}

	static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
	{
		using namespace ImGui;
		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID("##Splitter");
		ImRect bb;
		bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
		bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
		return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
	}


	Track& PushTrack(Context& context)
	{
		Track track;
		context.tracks.push_back(track);
		return context.tracks.back();
	}

	bool HasChannel(const Track& track, EventType type) {
		for (auto& channel : track.channels)
		{
			if (channel.type == type)
				return true;
		}
		return false;
	}


	Channel& PushChannel(Track& track, EventType type) {
		Channel channel = {};
		channel.type = type;
		track.channels.push_back(channel);
		return track.channels.back();
	}


	Event& PushEvent(Channel& channel) {
		Event event = {};
		channel.events.emplace_back(event);
		return channel.events.back();
	}


	void SetGlobalScale(float value)
	{
		global_scale = value;
	}

	float GetGlobalScale()
	{
		return global_scale;
	}


	/**
	 * @brief Init context
	 *
	 */
	void InitContext(Context& context) {
	}

	ImVec2 CalculateSize(Context& context) {
		auto total_height = 0;

		auto track_height = MAX(0.0f, CommonTheme.track_height * global_scale);
		total_height += context.tracks.size() * track_height;

		if (context.tracks.size() > 1) {
			total_height += (context.tracks.size() - 1) * CommonTheme.track_line_spacing * global_scale;
		}

		for (auto& track : context.tracks) {
			for (auto& channel : track.channels) {
				total_height += CommonTheme.event_height * global_scale;
			}

			total_height += track.channels.size() * CommonTheme.channel_line_spacing * global_scale;
		}

		float total_width = std::abs(context.state.range[1] - context.state.range[0]) * context.state.step_zoom;
		total_width += CommonTheme.channel_head_padding * global_scale;
		total_width += CommonTheme.channel_tail_padding * global_scale;

		return ImVec2(total_width, total_height);
	}

	float CalculateOffset(State& state, int time) {
		return (time - state.range[0]) * state.step_zoom;
	}

	/**
	 *  Cursor
	 *    __
	 *   |  |
	 *    \/
	 *
	 */
	void DrawTimeCursor(State& state, const ImVec2& B) {
		auto xMin = CalculateOffset(state, state.current_time);
		auto yMin = TimelineTheme.height * global_scale;

		xMin += B.x + state.pan[0];
		yMin += B.y;
				
		ImVec2 size{ 10.0f, 20.0f };
		auto topPos = ImVec2{ xMin, yMin };

		ImGui::PushID(0);
		ImGui::SetCursorPos(topPos - ImVec2{ size.x, size.y } -ImGui::GetWindowPos());
		ImGui::SetItemAllowOverlap(); // Prioritise the last drawn (shouldn't this be the default?)
		ImGui::InvisibleButton("##indicator", ImVec2(size.x * 2.0f, size.y));
		ImGui::PopID();

		static TimeType initial_time{ 0 };
		if (ImGui::IsItemActivated()) {
			initial_time = state.current_time;
		}

		ImVec4 color = TimelineTheme.cursor_color;
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			color = color * 1.2f;
		}

		if (ImGui::IsItemActive()) {
			auto newTime = initial_time + static_cast<int>(ImGui::GetMouseDragDelta().x / (state.step_zoom));
			newTime = MIN(newTime, state.range[1]);
			newTime = MAX(newTime, state.range[0]);
			state.current_time = newTime;
		}

		ImVec2 points[5] = {
			topPos,
			topPos - ImVec2{ -size.x, size.y / 2.0f },
			topPos - ImVec2{ -size.x, size.y },
			topPos - ImVec2{ size.x, size.y },
			topPos - ImVec2{ size.x, size.y / 2.0f }
		};

		ImVec2 shadow1[5];
		ImVec2 shadow2[5];
		for (int i = 0; i < 5; i++) { shadow1[i] = points[i] + ImVec2{ 1.0f, 1.0f }; }
		for (int i = 0; i < 5; i++) { shadow2[i] = points[i] + ImVec2{ 3.0f, 3.0f }; }

		auto painter = ImGui::GetWindowDrawList();
		painter->AddConvexPolyFilled(shadow1, 5, ImColor(TimelineTheme.cursor_shadow));
		painter->AddConvexPolyFilled(shadow2, 5, ImColor(TimelineTheme.cursor_shadow));
		painter->AddConvexPolyFilled(points, 5, ImColor(color));
		painter->AddPolyline(points, 5, ImColor(color * 1.25f), true, 1.0f);
		painter->AddLine(topPos - ImVec2{ 2.0f, size.y * 0.3f },
			topPos - ImVec2{ 2.0f, size.y * 0.8f },
			ImColor(EditorTheme.accent_dark));
		painter->AddLine(topPos - ImVec2{ -2.0f, size.y * 0.3f },
			topPos - ImVec2{ -2.0f, size.y * 0.8f },
			ImColor(EditorTheme.accent_dark));
	};

	void DrawTrackHeader(const Track& track, ImVec2& cursor, const ImVec2& winSize, const ImVec2& realSize) {
		// Draw track header, a separator-like empty space
		// 
		// |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
		// |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
		// |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
		// |                                         |
		// |_________________________________________|
		// |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
		// |__|__|__|__|__|__|__|__|__|__|__|__|__|__|
		// |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
		//
		//const ImVec2 size{ winSize.x - CommonTheme.channel_head_padding * global_scale - CommonTheme.channel_tail_padding * global_scale, CommonTheme.track_height * global_scale };
		const ImVec2 size{ realSize.x - CommonTheme.channel_head_padding * global_scale - CommonTheme.channel_tail_padding * global_scale, CommonTheme.track_height * global_scale };
		auto* painter = ImGui::GetWindowDrawList();
		// Hide underlying vertical lines
		painter->AddRectFilled(cursor, cursor + size, ImColor(EditorTheme.active_area_background));
		
		if (CommonTheme.tint_track) {
			// Tint empty area with the track color
			painter->AddRectFilled(cursor, cursor +size, ImColor(track.color.x, track.color.y, track.color.z, 0.1f));
		}

		painter->AddRect(cursor, cursor +size, ImColor(EditorTheme.mid));

		cursor.y += size.y;
	}

	void DrawEvents(Context& context, const ImVec2& winSize, const ImVec2& realSize) {
		auto& state = context.state;

		auto* painter = ImGui::GetWindowDrawList();
		ImVec2 cursor = ImGui::GetCursorScreenPos();
		cursor.x += CommonTheme.channel_head_padding * global_scale;

		unsigned int track_count{ 0 };
		for (auto& track : context.tracks) {
			track_count++;
			if (CommonTheme.track_height * global_scale > 0) {
				DrawTrackHeader(track, cursor, winSize, realSize);
			}

			// Give each event a unique ImGui ID
			unsigned int channel_count{ 0 };
			unsigned int event_count{ 0 };

			// Draw events
			//            ________
			//           |________|
			//  _________________________
			// |_________________________|
			//     ________      ____________________________
			//    |________|    |____________________________|
			//
			for (auto& channel : track.channels) {
				channel_count++;
				cursor.y += CommonTheme.channel_line_spacing * global_scale;

				event_count = 0;
				for (auto& event : channel.events) {
					assert(event.length > 0);
					event_count++;

					ImVec2 pos{ CalculateOffset(state, event.time), 0.0f };
					ImVec2 size{ event.length * state.step_zoom, CommonTheme.event_height * global_scale };
					ImVec2 head_size{ 10.0f, size.y };
					ImVec2 tail_size{ 10.0f, size.y };

					// Transitions
					float target_height{ 0.0f };
					float target_thickness = 0.0f;

					/** TODO: Implement crop interaction and visualisation
						 ____________________________________
						|      |                      |      |
						| head |         body         | tail |
						|______|______________________|______|

					*/

					ImGui::PushID(track.label.c_str());
					ImGui::PushID(channel_count * 10000 + event_count);
					ImGui::SetCursorScreenPos(cursor + pos);
					ImGui::SetItemAllowOverlap();
					if (ImGui::InvisibleButton("##event", size))
					{
						if (event.click)
							event.click(ImGui::IsMouseClicked(0), &event);
						
						if(event.on_right_click_gui && ImGui::IsMouseReleased(1))
							ImGui::OpenPopup("right_click_context");
					}

					if (event.on_right_click_gui && ImGui::BeginPopup("right_click_context"))
					{
						event.on_right_click_gui(&event);
						ImGui::EndPopup();
					}

					ImGui::PopID();
					ImGui::PopID();

					ImVec4 color = event.color;

					if (!event.enabled || track.mute || track._notsoloed) {
						color = ImColor::HSV(0.0f, 0.0f, 0.5f);
					}

					else if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
						target_thickness = 2.0f;
					}

					// User Input
					static int initial_time{ 0 };

					if (ImGui::IsItemActivated()) {
						initial_time = event.time;
						state.selected_event = &event;
					}

					if (ImGui::IsItemActive()) {
						if (event.canmove)
						{
							float delta = ImGui::GetMouseDragDelta().x;
							int new_time = initial_time + delta / state.step_zoom;

							new_time = MAX(new_time, event.move_min);
							new_time = MIN(new_time, event.move_max - event.length + 1);

							pos.x = CalculateOffset(state, new_time);

							if (!channel.can_overlap) {
								for (auto& other : channel.events) {
									if (&other != &event){
										if (new_time + event.length <= other.time || other.time + other.length <= new_time) {
										}
										else {
											new_time = event.time;
											break;
										}
									}
								}
							}

							if (event.time != new_time)
							{
								bool change = true;
								if (context.state.onEventMoveTimeCondition)
								{
									change = context.state.onEventMoveTimeCondition(track_count - 1, channel_count - 1, event_count - 1, new_time);
								}
								if (change)
								{
									event.time = new_time;
									if(context.state.onEventMoveTime)
										context.state.onEventMoveTime(track_count - 1, channel_count - 1, event_count - 1, new_time);
								}
							}
						}
						target_height = 5.0f;
					}

					_transition(event.height, target_height, CommonTheme.transition_speed);
					pos -= event.height;

					const int shadow = 2;
					painter->AddRectFilled(
						cursor + pos + shadow + event.height * 1.25f,
						cursor + pos + size + shadow + event.height * 1.25f,
						ImColor::HSV(0.0f, 0.0f, 0.0f, 0.3f), CommonTheme.event_radius
					);

					painter->AddRectFilled(
						cursor + pos,
						cursor + pos + size,
						ImColor(color), CommonTheme.event_radius
					);

					// Add a dash to the bottom of each event.
					painter->AddRectFilled(
						cursor + pos + ImVec2{ 0.0f, size.y - 5.0f },
						cursor + pos + size,
						ImColor(color * 0.8f), CommonTheme.event_radius
					);

					if (ImGui::IsItemHovered() || ImGui::IsItemActive() || state.selected_event == &event) {
						painter->AddRect(
							cursor + pos + event.thickness * 0.25f,
							cursor + pos + size - event.thickness * 0.25f,
							ImColor(EditorTheme.selection), CommonTheme.event_radius, ImDrawCornerFlags_All, event.thickness
						);
					}
					else {
						painter->AddRect(
							cursor + pos + event.thickness,
							cursor + pos + size - event.thickness,
							ImColor(EditorTheme.outline), CommonTheme.event_radius
						);
					}

					if (event.enabled) {
						if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
							if (event.length >= 1.0f) {
								painter->AddText(
									ImGui::GetFont(),
									ImGui::GetFontSize() * 0.85f,
									cursor + pos + ImVec2{ 3.0f + event.thickness, 0.0f },
									ImColor(EditorTheme.text),
									std::to_string(event.time).c_str()
								);
							}

							if (event.length >= 5.0f) {
								painter->AddText(
									ImGui::GetFont(),
									ImGui::GetFontSize() * 0.85f,
									cursor + pos + ImVec2{ size.x - 20.0f, 0.0f },
									ImColor(EditorTheme.text),
									std::to_string(event.length).c_str()
								);
							}
						}
					}
				}

				// Next event type
				cursor.y += CommonTheme.event_height * global_scale;
			}

			// Next track
			if (track_count < context.tracks.size()) {
				cursor.y += CommonTheme.track_line_spacing * global_scale;
			}
		}
	}

	void DrawContext(Context& context) {
		auto& state = context.state;

		/**
		 * @brief Sequentity divided into 4 panels
		 *
		 *          _________________________________________________
		 *         |       |                                         |
		 *   Cross |   X   |                  B                      | Timeline
		 *         |_______|_________________________________________|
		 *         |       |                                         |
		 *         |       |                                         |
		 *         |       |                                         |
		 *  Lister |   A   |                  C                      | Editor
		 *         |       |                                         |
		 *         |       |                                         |
		 *         |_______|_________________________________________|
		 *
		 */
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
		if (ImGui::BeginChild("##sequentity_scrolling_region"))
		{
			static float leftPaneWidth = 200.0f;
			static float rightPaneWidth = 800.0f;
			const float splitter_width = 4.0f;
			Splitter(true, splitter_width, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, TimelineTheme.background);
				bool show = ImGui::BeginChild("sequentity_left_panel", ImVec2(leftPaneWidth - splitter_width, 0.0f));
				ImGui::PopStyleColor();

				if (show)
				{
					auto winHeight = ImGui::GetWindowHeight();
					if (ImGui::BeginChild("sequentity_left_top", ImVec2(0.0f, TimelineTheme.height * global_scale)))
					{
						if (context.state.leftTopGUI)
						{
							context.state.leftTopGUI();
						}
					}
					ImGui::EndChild();

					ImGui::Separator();

					auto bottomHeight = winHeight - TimelineTheme.height * global_scale - ImGui::GetStyle().ScrollbarSize - ImGui::GetItemRectSize().y;
					if (bottomHeight > 5.0f) {

						ImGui::PushStyleColor(ImGuiCol_FrameBg, EditorTheme.left_panel_background);
						show = ImGui::BeginChildFrame(ImGui::GetID("sequentity_left_bottom"), ImVec2(0.0f, bottomHeight), ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar);
						ImGui::PopStyleColor();

						if (show) {
							ImGui::SetScrollY(-state.pan[1]);
							unsigned int track_count{ 0 };
							for (auto& track : context.tracks) {
								track_count++;
								if (CommonTheme.track_height * global_scale > 0) {
									ImGui::PushID(track.label.c_str());

									if (ImGui::BeginChildFrame(track_count, ImVec2(0.0f, CommonTheme.track_height * global_scale)), ImGuiWindowFlags_NoBackground) {
										ImGui::Text(track.label.c_str());
									}
									ImGui::EndChildFrame();

									ImGui::PopID();
								}

								// Give each event a unique ImGui ID
								unsigned int channel_count{ 0 };

								for (auto& channel : track.channels) {
									channel_count++;
									ImGui::Dummy(ImVec2(0.0f, CommonTheme.channel_line_spacing * global_scale));

									if (channel.is_active)
									{
										ImGui::PushStyleColor(ImGuiCol_FrameBg, EditorTheme.active_channel_background);
										show = ImGui::BeginChildFrame(track_count * 10000 + channel_count, ImVec2(0.0f, CommonTheme.event_height * global_scale), 0);
										ImGui::PopStyleColor();
									}
									else
									{
										show = ImGui::BeginChildFrame(track_count * 10000 + channel_count, ImVec2(0.0f, CommonTheme.event_height * global_scale), ImGuiWindowFlags_NoBackground);
									}
									if (show) {
										ImVec2 curPos = ImGui::GetCursorScreenPos();

										auto framePadding = (CommonTheme.event_height * global_scale - ImGui::GetFontSize()) * 0.5f;
										ImGui::SetCursorScreenPos(ImVec2(curPos.x, curPos.y + framePadding));
										ImGui::Text(channel.label.c_str());
										ImGui::SetCursorScreenPos(curPos);

										ImVec2 min(ImGui::GetWindowWidth() - CommonTheme.event_height * global_scale * 0.9f, CommonTheme.event_height * global_scale * 0.1f);
										ImVec2 max(min.x + CommonTheme.event_height * global_scale * 0.8f, min.y + CommonTheme.event_height * global_scale * 0.8f);
										min = min + curPos;
										max = max + curPos;
										ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImColor(channel.color));
										ImGui::GetWindowDrawList()->AddRect(min, max, ImColor(channel.color * 1.25f));

										if (ImGui::InvisibleButton("touch", ImGui::GetWindowSize())) {
											//CCLOG("click left track_count : %d, channel_count: %d", track_count, channel_count);
											if (state.onClickChannel) {
												state.onClickChannel(track_count, channel_count, &channel);
											}
										}
									}
									ImGui::EndChildFrame();

								}
								if (track_count < context.tracks.size()) {
									ImGui::Dummy(ImVec2(0.0f, CommonTheme.track_line_spacing * global_scale));
								}
							}
						}
						ImGui::EndChildFrame();
					}
				}
				ImGui::EndChild();
			}

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(splitter_width * 3.0f, 0.0f));
			ImGui::SameLine();


			if (ImGui::BeginChild("sequentity_right_panel", ImVec2(0.0f, 0.0f), ImGuiWindowFlags_NoBackground))
			{
				if (ImGui::GetContentRegionAvailWidth() > 5) {
					ImVec2 curPos = ImGui::GetCursorScreenPos();
					auto* painter = ImGui::GetWindowDrawList();
					// top 
					painter->AddRectFilled(
						curPos,
						curPos + ImVec2(ImGui::GetContentRegionAvailWidth(), TimelineTheme.height * global_scale),
						ImColor(TimelineTheme.background)
					);


					// line
					painter->AddLine(
						curPos + ImVec2(0, TimelineTheme.height * global_scale),
						curPos + ImVec2(ImGui::GetContentRegionAvailWidth(), TimelineTheme.height * global_scale),
						ImGui::GetColorU32(ImGuiCol_Separator));


					auto B = curPos;
					// timeline
					for (TimeType i = state.range[0]; i <= state.range[1]; ++i) {

						float xMin = CalculateOffset(state, i) + CommonTheme.channel_head_padding * global_scale;
						float xMax = 0.0f;
						float yMin = 0.0f;
						float yMax = TimelineTheme.height * global_scale - 1;

						xMin += state.pan[0];
						xMin += B.x;

						xMax += state.pan[0];
						xMax += B.x;

						yMin += B.y;
						yMax += B.y;

						if (i % state.stride == 0) {
							painter->AddLine(ImVec2(xMin, yMin), ImVec2(xMin, yMax), ImColor(TimelineTheme.dark));
							painter->AddText(
								ImGui::GetFont(),
								ImGui::GetFontSize() * 0.85f,
								ImVec2{ xMin + 5.0f, yMin },
								ImColor(TimelineTheme.text),
								std::to_string(i).c_str()
							);
						}
						else {
							if (i != state.current_time) {
								painter->AddLine(ImVec2{ xMin, yMin + (TimelineTheme.height * global_scale * 0.5f) }, ImVec2{ xMin, yMax }, ImColor(TimelineTheme.mid));
							}
						}
					}

					DrawTimeCursor(state, ImVec2(B.x + CommonTheme.channel_head_padding * global_scale, B.y));
				}
				else {
					ImGui::Dummy(ImVec2(0.0f, TimelineTheme.height * global_scale));
				}

				if (ImGui::BeginChildFrame(ImGui::GetID("sequentity_right_panel"), ImVec2(0.0f, 0.0f), ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysHorizontalScrollbar))
				{
					auto realSize = CalculateSize(context);

					ImVec2 child_size = ImGui::GetWindowSize();
					child_size.y = MAX(child_size.y, realSize.y);
					child_size.x = MAX(child_size.x, realSize.x);

					// C
					auto C = ImGui::GetCursorScreenPos();
					auto* painter = ImGui::GetWindowDrawList();

					// head inactive area background
					auto area_min = C;
					auto area_max = ImVec2(C.x + CommonTheme.channel_head_padding * global_scale, C.y + child_size.y);
					painter->AddRectFilled(area_min, area_max, ImColor(EditorTheme.inactive_area_background));

					// active area background
					area_min = ImVec2(C.x + CommonTheme.channel_head_padding * global_scale, C.y);
					area_max = ImVec2(C.x + (realSize.x - CommonTheme.channel_tail_padding * global_scale), C.y + child_size.y);
					painter->AddRectFilled(area_min, area_max, ImColor(EditorTheme.active_area_background));

					// tail inactive area background
					area_min = ImVec2(C.x + realSize.x - CommonTheme.channel_tail_padding * global_scale, C.y);
					area_max = ImVec2(C.x + child_size.x, C.y + child_size.y);
					painter->AddRectFilled(area_min, area_max, ImColor(EditorTheme.inactive_area_background));


					// draw active track background
					float curPosY = 0.0f;
					unsigned int track_count{ 0 };
					for (auto& track : context.tracks) {
						track_count++;

						if (CommonTheme.track_height * global_scale > 0) {
							curPosY += CommonTheme.track_height * global_scale;
						}

						unsigned int channel_count{ 0 };
						for (auto& channel : track.channels) {
							channel_count++;
							curPosY += CommonTheme.channel_line_spacing * global_scale;

							if (channel.is_active)
							{
								painter->AddRectFilled(
									C + ImVec2(0.0f, curPosY),
									C + ImVec2(realSize.x - CommonTheme.channel_tail_padding * global_scale, curPosY + CommonTheme.event_height * global_scale),
									ImColor(EditorTheme.active_channel_background)
								);
							}
							curPosY += CommonTheme.event_height * global_scale;
							if (track_count < context.tracks.size()) {
								curPosY += CommonTheme.track_line_spacing * global_scale;
							}
						}
					}

					// lines
					for (TimeType i = state.range[0]; i < state.range[1]; ++i) {
						float xMin = CalculateOffset(state, i) + CommonTheme.channel_head_padding * global_scale;
						float yMin = 0.0f;
						float yMax = child_size.y;

						xMin += C.x;
						yMin += C.y;
						yMax += C.y;

						if (i != state.current_time) {
							if (i % state.stride == 0) {
								painter->AddLine(ImVec2(xMin, yMin), ImVec2(xMin, yMax), ImColor(EditorTheme.dark));
							}
							else {
								painter->AddLine(ImVec2(xMin, yMin), ImVec2(xMin, yMax), ImColor(EditorTheme.mid));
							}
						}
					}


					ImGui::Dummy(child_size);
					//ImGui::InvisibleButton("editor_canvas", child_size , ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
					ImGui::SetCursorScreenPos(C);

					// events
					DrawEvents(context, child_size, realSize);
					ImGui::SetCursorScreenPos(C);

					// current line
					if (state.range[0] <= state.current_time && state.current_time <= state.range[1])
					{
						float xMin = CalculateOffset(state, state.current_time) + CommonTheme.channel_head_padding * global_scale;
						float yMin = 0.0f;
						float yMax = child_size.y;
						xMin += C.x;
						yMin += C.y;
						yMax += C.y;
						painter->AddLine(ImVec2(xMin, yMin), ImVec2(xMin, yMax), ImColor(EditorTheme.current_time), 2.0f);
					}

					state.pan[0] = -ImGui::GetScrollX();
					state.pan[1] = -ImGui::GetScrollY();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar(3);
	}

	void ThemeEditor(bool* p_open) {
		ImGui::Begin("Theme", p_open);
		{
			if (ImGui::CollapsingHeader("Common")) {

				ImGui::DragFloat("transition_speed##global", &CommonTheme.transition_speed, 0.1f, 0.0f, 1.0f);
				ImGui::DragFloat("track_height##global", &CommonTheme.track_height, 1.0f, 0.0f, 100.0f);
				ImGui::DragFloat("event_height##global", &CommonTheme.event_height, 1.0f, 5.0f, 100.0f);
				ImGui::DragFloat("channel_line_spacing##global", &CommonTheme.channel_line_spacing, 1.0f, 0.0f, 100.0f);
				ImGui::DragFloat("track_line_spacing##global", &CommonTheme.track_line_spacing, 1.0f, 0.0f, 100.0f);
				ImGui::DragFloat("channel_head_padding##global", &CommonTheme.channel_head_padding, 1.0f, 0.0f, 1000.0f);
				ImGui::DragFloat("channel_tail_padding##global", &CommonTheme.channel_tail_padding, 1.0f, 0.0f, 1000.0f);
				ImGui::DragFloat("event_radius##global", &CommonTheme.event_radius);
			}

			if (ImGui::CollapsingHeader("Timeline")) {
				ImGui::ColorEdit4("background", &TimelineTheme.background.x);
				ImGui::ColorEdit4("text", &TimelineTheme.text.x);
				ImGui::ColorEdit4("dark", &TimelineTheme.dark.x);
				ImGui::ColorEdit4("mid", &TimelineTheme.mid.x);

				ImGui::ColorEdit4("cursor_shadow", &TimelineTheme.cursor_shadow.x);
				ImGui::ColorEdit4("cursor_color", &TimelineTheme.cursor_color.x);

				ImGui::DragFloat("height", &TimelineTheme.height, 1.0f, 0.0f, 50.0f);
			}
			
			if (ImGui::CollapsingHeader("Editor")) {
				ImGui::ColorEdit4("background##editor", &EditorTheme.background.x);
				ImGui::ColorEdit4("left_panel_background##editor", &EditorTheme.left_panel_background.x);
				ImGui::ColorEdit4("inactive_area_background##editor", &EditorTheme.inactive_area_background.x);
				ImGui::ColorEdit4("active_area_background##editor", &EditorTheme.active_area_background.x);
				ImGui::ColorEdit4("active_channel_background##editor", &EditorTheme.active_channel_background.x);
				ImGui::ColorEdit4("text##editor", &EditorTheme.text.x);
				ImGui::ColorEdit4("mid##editor", &EditorTheme.mid.x);
				ImGui::ColorEdit4("dark##editor", &EditorTheme.dark.x);
				ImGui::ColorEdit4("accent_dark##editor", &EditorTheme.accent_dark.x);
				ImGui::ColorEdit4("selection##editor", &EditorTheme.selection.x);
				ImGui::ColorEdit4("outline##editor", &EditorTheme.outline.x);
				ImGui::ColorEdit4("current_time##editor", &EditorTheme.current_time.x);
			}
		}
		ImGui::End();
	}

} // namespace Sequentity

/*

======================= Jefferson Licence ===========================

In short, it's BSD 2-clause, except I get a copy of what you make.

For example, if the source code is used in the development of the next
Grand Theft Auto, send me a copy. Used in your next music application?
Send me a copy. In a Hollywood blockbuster? Send me a copy. Seems fair? =)

=====================================================================

Copyright (c) 2020, Alan Jefferson

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Copyright holder is entitled to a free copy of anything produced using this
   source code.
2. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
3. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/