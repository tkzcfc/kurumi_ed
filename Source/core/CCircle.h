#pragma once

#include "axmol.h"

USING_NS_AX;

class CCircle
{
public:

	CCircle();

	void setCenter(float x, float y);

	const Vec2& getCenter();

	void setRadius(float value);

	float getRadius();


	void debugDraw(DrawNode* pDrawNode);

	void setAlpha(float alpha);

	void setVisible(bool value);

	bool isVisible();

	void setColor(unsigned int color);

	unsigned int getColor() { return m_color_Uint; }

protected:
	bool m_isVisible;

	Vec2 m_center;
	float m_radius;

	Color4F m_color;
	unsigned int m_color_Uint;

	float m_alpha;
};
