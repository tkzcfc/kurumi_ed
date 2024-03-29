#pragma once

#include "CSequentity.h"

CSequentity::CSequentity()
{
	Sequentity::InitContext(m_context);

	m_context.state.leftTopGUI = [this]() 
	{
		auto handle = this->getLuaHandle("onLeftTopGUI");
		if (handle && handle->isvalid())
		{
			handle->ppush();
			handle->pcall();
		}
	};

	m_context.state.onEventMoveTime = [this](int track_count, int channel_count, int event_count, int time)
	{
		auto handle = this->getLuaHandle("onEventMoveTime");
		if (handle && handle->isvalid())
		{
			handle->ppush();
			handle->pusharg(track_count);
			handle->pusharg(channel_count);
			handle->pusharg(event_count);
			handle->pusharg(time);
			handle->pushusertype<Sequentity::Channel>(&m_context.tracks[track_count].channels[channel_count], "Sequentity::Channel");
			handle->pcall();
		}
	};

	m_context.state.onEventMoveTimeCondition = [this](int track_count, int channel_count, int event_count, int time)
	{
		auto handle = this->getLuaHandle("onEventMoveTimeCondition");
		if (handle && handle->isvalid())
		{
			handle->ppush();
			handle->pusharg(track_count);
			handle->pusharg(channel_count);
			handle->pusharg(event_count);
			handle->pusharg(time);
			handle->pushusertype<Sequentity::Channel>(&m_context.tracks[track_count].channels[channel_count], "Sequentity::Channel");

			

			LuaRetValue value;

			if (handle->pcall(&value, 1))
			{
				return value.getBool();
			}
			return false;
		}
		return true;
	};

	m_context.state.onClickChannel = [this](int track_count, int channel_count, Sequentity::Channel* channel)
	{
		auto handle = this->getLuaHandle("onClickChannel");
		if (handle && handle->isvalid())
		{
			handle->ppush();
			handle->pusharg(track_count);
			handle->pusharg(channel_count);
			handle->pushusertype<Sequentity::Channel>(channel, "Sequentity::Channel");
			handle->pcall();
		}
	};
}

CSequentity::~CSequentity()
{}

Sequentity::Track* CSequentity::addTrack()
{
	return &Sequentity::PushTrack(m_context);
}

Sequentity::Channel* CSequentity::addChannel(Sequentity::Track* track, Sequentity::EventType type)
{
	assert(track != NULL);
	CCLOG("add channel type = %d", type);
	return &Sequentity::PushChannel(*track, type);
}

Sequentity::Event* CSequentity::addEvent(Sequentity::Channel* channel)
{
	return &Sequentity::PushEvent(*channel);
}

void CSequentity::clearEvent(Sequentity::Channel* channel)
{
	m_context.state.selected_event = NULL;
	channel->events.clear();
}

void CSequentity::clearChanel(Sequentity::Track* track)
{
	m_context.state.selected_event = NULL;
	track->channels.clear();
}

void CSequentity::clearTrack()
{
	m_context.state.selected_event = NULL;
	m_context.tracks.clear();
}

void CSequentity::removEvent(Sequentity::Event* event)
{
	for (auto& track : m_context.tracks)
	{
		for (auto & channel : track.channels)
		{
			for (auto it = channel.events.begin(); it != channel.events.end(); ++it)
			{
				auto& curEvent = *it;
				if (&curEvent == event)
				{
					channel.events.erase(it);
					break;
				}
			}
		}
	}
}

std::vector<Sequentity::Track*> CSequentity::getAllTrack()
{
	std::vector<Sequentity::Track*> out;
	out.reserve(m_context.tracks.size());

	for (auto& it : m_context.tracks)
	{
		out.push_back(&it);
	}

	return out;
}

std::vector<Sequentity::Channel*> CSequentity::getAllChannel(Sequentity::Track* track)
{
	assert(track != NULL);

	std::vector<Sequentity::Channel*> out;

	for (auto& it : track->channels)
	{
		out.push_back(&it);
	}

	return out;
}

std::vector<Sequentity::Event*> CSequentity::getAllEvent(Sequentity::Channel* channel)
{
	assert(channel != NULL);
	
	std::vector<Sequentity::Event*> out;
	out.reserve(channel->events.size());

	for (auto& it : channel->events)
	{
		out.push_back(&it);
	}

	return out;
}

void CSequentity::onGUI()
{
	Sequentity::DrawContext(m_context);

	//Sequentity::Intersect(track, context.state.current_time, [&](auto& event) {
	//	if (event.type == event_1) {
	//		auto& data = *static_cast<EventData*>(event.data);
	//		//CCLOG("in event %d", data.number);
	//	}
	//});
}

void CSequentity::setRange(int min, int max)
{
	m_context.state.range[0] = MIN(min, max);
	m_context.state.range[1] = MAX(min, max);
}

int CSequentity::getMax()
{
	return m_context.state.range[1];
}

int CSequentity::getMin()
{
	return m_context.state.range[0];
}

void CSequentity::setCurrentTime(int current_time)
{
	m_context.state.current_time = current_time;
}

int CSequentity::getCurrentTime()
{
	return m_context.state.current_time;
}

