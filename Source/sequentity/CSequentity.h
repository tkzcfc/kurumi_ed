#pragma once

#include "cocos2d.h"
#include "Sequentity.h"
#include "lua_function/LuaFunctionBond.h"

class CSequentity : public LuaFunctionBond
{
public:

	CSequentity();

	virtual ~CSequentity();

	void onGUI();

	Sequentity::Track* addTrack();

	Sequentity::Channel* addChannel(Sequentity::Track* track, Sequentity::EventType type);

	Sequentity::Event* addEvent(Sequentity::Channel* channel);

	void clearEvent(Sequentity::Channel* channel);

	void clearChanel(Sequentity::Track* track);

	void clearTrack();


	void removEvent(Sequentity::Event* event);


	std::vector<Sequentity::Track*> getAllTrack();

	std::vector<Sequentity::Channel*> getAllChannel(Sequentity::Track* track);

	std::vector<Sequentity::Event*> getAllEvent(Sequentity::Channel* channel);


	void setRange(int min, int max);

	int getMax();

	int getMin();

	void setCurrentTime(int current_time);

	int getCurrentTime();

protected:

	Sequentity::Context m_context;
};

