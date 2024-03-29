/**

Sequentity.h - v0.1 - A single-file, immediate-mode sequencer widget for C++17, Dear ImGui and EnTT

See license at the bottom of this file.

Do this:

    #define SEQUENTITY_IMPLEMENTATION

before you include this file in *one* C++ (17 and above) file to create the implementation.

*/

#ifndef SEQUENTITY_H
#define SEQUENTITY_H

#include <vector>
#include <unordered_map>
#include <functional> // std::function
#include <string>     // std::to_string

// Originally made with ImGui-1.74 (docking)
#include <imgui.h>

namespace Sequentity {

using EventType = std::int32_t;
using TimeType = int;            // This isn't really editable

// Some example events
enum EventType_ : EventType {
    EventType_NONE = -1,
};

/**
 * @brief A Sequentity Event
 *
 */
struct Event {
    TimeType time { 0 };
    TimeType length { 0 };

    ImVec4 color { ImColor::HSV(0.33f, 0.75f, 0.75f) };

    // Map your custom data here, along with an optional type
    EventType type { EventType_NONE };
    void* data { nullptr };
	
    /* Whether or not to consider this event */
    bool enabled { true };
	
    /* Extend or reduce the length of an event */
    float scale { 1.0f };
	
	bool canmove { true };

	int move_min{ INT_MIN };
	int move_max{ INT_MAX };

	void(*click)(bool, Event*) { nullptr };
	void(*on_right_click_gui)(Event*) { nullptr };

	// Visuals, animation
	float height{ 0.0f };
	float thickness{ 0.0f };
};


/**
 * @brief A collection of events
 *
 */
struct Channel {
    std::string label { "Untitled channel" };

    ImVec4 color { ImColor::HSV(0.33f, 0.5f, 1.0f) };

    std::vector<Event> events;

	bool can_overlap{ false };
	
	bool is_active{ false };

    EventType type{ 0 };
};


/**
 * @brief A collection of channels
 *
 */
struct Track {
    std::string label { "Untitled track" };

    ImVec4 color { ImColor::HSV(0.66f, 0.5f, 1.0f) };

    bool solo { false };
    bool mute { false };

    //std::unordered_map<EventType, Channel> channels;
    std::vector<Channel> channels;
    
    // Internal
    bool _notsoloed { false };
};


/**
 * Tag indicating whether a track is selected
 *
 * This can come in handy if you implement selection in your application
 * and wish to synchronise the currently selected track with its associated entity
 *
 */
struct Selected {};

/**
 * @brief Lambda signatures
 *
 */
using IntersectFunc = std::function<void(const Event&)>;
using IntersectWithPreviousFunc = std::function<void(const Event*, const Event&)>;
using IntersectWithPreviousAndNextFunc = std::function<void(const Event&, const Event&, const Event&)>;


/**
 * @brief All of Sequentities (mutable) state
 *
 * Accessible from your application via registry.ctx<Sequentity::State>();
 *
 */
struct State {

    // Functional
    int current_time { 0 };
    int range[2] { -11, 100 };

    // Selection
    Event*   selected_event { nullptr };

    // Visual
    float pan[2] { 8.0f, 8.0f };
    int stride { 5 };
	float step_zoom{ 25.0f };

	//float event_height{ 20.0f };

    // Transitions
    float target_zoom[2] { 200.0f, 20.0f };
    float target_pan[2] { 15.0f, 20.0f };

	std::function<void()> leftTopGUI = nullptr;

	std::function<bool(int, int, int, int)> onEventMoveTimeCondition = nullptr;
	std::function<void(int, int, int, int)> onEventMoveTime = nullptr;

	std::function<void(int, int, Channel*)> onClickChannel = nullptr;
};

/**
 * @brief FC-FIX
 *
*/
struct Context
{
	State state;
	std::vector<Track> tracks;
};



/**
 * @brief Init context
 *
 */
void InitContext(Context& context);

/**
 * @brief Draw context
 *
 */
void DrawContext(Context& context);


/**
 * @brief Optional theme editor
 *
 */
void ThemeEditor(bool* p_open = nullptr);


Track& PushTrack(Context& context);

/**
 * @brief Yield all events that intersect the given track at the given time
 *
 * Use this to iterate over relevant events,
 * passing in a function to operate on the result.
 *
 *                  time
 *                    |
 *       _____________|__________   ______
 *      |_____________|__________| |______
 *             _______|__________       __
 *            |_______|__________|     |__
 *       _____________|___
 *      |_____________|___|
 *      ^             |
 *      .             |
 *      .              
 *    event            
 *
 *
 */
void Intersect(const Track& track, TimeType time, IntersectFunc func);
void Intersect(const Track& track, TimeType time, IntersectWithPreviousFunc func);

bool HasChannel(const Track& track, EventType type);
Channel& PushChannel(Track& track, EventType type);
Event&   PushEvent(Channel& channel);

void SetGlobalScale(float value);
float GetGlobalScale();

} // namespace Sequentity

#endif /* SEQUENTITY_H */


/**

Documentation
=============

- ~2,000 vertices with 15 events, about 1,500 without any
- EnTT is a entity-component-system (ECS) framework used to host the data

# Layout

 ___________ __________________________________________________
|           |                                                  |
|-----------|--------------------------------------------------|
| Track     |                                                  |
|   Channel |  Event Event Event                               |
|   Channel |  Event Event Event                               |
|   Channel |  Event Event Event                               |
|   ...     |  ...                                             |
|___________|__________________________________________________|

# Legend

- Event:   An individual coloured bar, with a start, length and additional metadata
- Channel: A vector of Events
- Track:   A vector of Channels

*/

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