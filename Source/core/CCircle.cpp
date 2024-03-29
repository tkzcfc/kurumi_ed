
#include "CCircle.h"
#include "imgui.h"


CCircle::CCircle()
{
	setColor(IM_COL32(255, 0, 0, 255));
	setVisible(true);

	m_alpha = 1.0f;
	m_radius = 10.0f;
}

void CCircle::setCenter(float x, float y)
{
	m_center.x = x;
	m_center.y = y;
}

const Vec2& CCircle::getCenter()
{
	return m_center;
}

void CCircle::setRadius(float value)
{
	m_radius = value;
}

float CCircle::getRadius()
{
	return m_radius;
}

void CCircle::debugDraw(DrawNode* pDrawNode)
{
	if (!m_isVisible || pDrawNode == NULL)
	{
		return;
	}
	   
	Color4F color = m_color;
	color.a *= m_alpha;

	Color4F fillColor = color;
	fillColor.a *= 0.5f;


	//pDrawNode->drawCircle(m_center, m_radius, 0.0f, 100, false, color);

	const int segments = 50;
	const float coef = 2.0f * (float)M_PI / segments;

	Vec2* vertices = new (std::nothrow) Vec2[segments + 2];
	if (!vertices)
		return;

	for (unsigned int i = 0; i <= segments; i++) {
		float rads = i * coef;
		GLfloat j = m_radius * cosf(rads) + m_center.x;
		GLfloat k = m_radius * sinf(rads) + m_center.y;

		vertices[i].x = j;
		vertices[i].y = k;
	}

	pDrawNode->drawPolygon(vertices, segments, fillColor, 1.0f, color);

	delete[]vertices;
	vertices = NULL;

}

void CCircle::setAlpha(float alpha)
{
	m_alpha = alpha;
}

void CCircle::setVisible(bool value)
{
	m_isVisible = value;
}

bool CCircle::isVisible()
{
	return m_isVisible;
}

void CCircle::setColor(unsigned int color)
{
	float sc = 1.0f / 255.0f;
	m_color.r = (float)((color >> IM_COL32_R_SHIFT) & 0xFF) * sc;
	m_color.g = (float)((color >> IM_COL32_G_SHIFT) & 0xFF) * sc;
	m_color.b = (float)((color >> IM_COL32_B_SHIFT) & 0xFF) * sc;
	m_color.a = (float)((color >> IM_COL32_A_SHIFT) & 0xFF) * sc;

	m_color_Uint = color;
}
