#pragma once

#include "axmol.h"

USING_NS_AX;

#define POLYGON_VERTEXT_COUNT_MAX 8

class CPolygon
{
public:

	CPolygon();

	virtual ~CPolygon();

	void setVertexCount(int count);

	int getVertexCount();

	void setVertex(const Vec2& point, int index);

	Vec2 getVertex(int index);


	void debugDraw(DrawNode* pDrawNode);

	void setAlpha(float alpha);

	void setColor(unsigned int color);

	unsigned int getColor() { return m_color_Uint; }

	void setVisible(bool value);

public:

	Vec2 m_vertexs[POLYGON_VERTEXT_COUNT_MAX];
	int m_count;

	float m_alpha;
	Color4F m_color;
	unsigned int m_color_Uint;
	bool m_isVisible;
};
