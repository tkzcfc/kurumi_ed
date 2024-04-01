#pragma once

#include "axmol.h"
#include "ilayer/ILayer.h"
#include "lua_function/LuaFunctionBond.h"

USING_NS_AX;

class EditorContext : public Node, public LuaFunctionBond
{
public:

	EditorContext();

	virtual ~EditorContext();

	virtual bool init()override;

	CREATE_FUNC(EditorContext);

	void testSequentity();

private:

	void onGUI();

	void callLuaGUI(const char* name);

private:

	Node* m_backgroundNode;

	bool m_initGUITag;

    bool m_openDockspace;
    bool m_redock;
};
