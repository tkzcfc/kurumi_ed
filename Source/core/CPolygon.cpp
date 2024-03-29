#include "CPolygon.h"
#include "imgui.h"
#include "Triangulate.h"

CPolygon::CPolygon()
	: m_count(3)
	, m_alpha(0.7f)
	, m_isVisible(true)
{
	setColor(IM_COL32(255, 0, 0, 255));
}

CPolygon::~CPolygon()
{}

void CPolygon::setVertexCount(int count)
{
	assert(m_count > 2 && m_count <= POLYGON_VERTEXT_COUNT_MAX);
	m_count = count;
}

int CPolygon::getVertexCount()
{
	return m_count;
}

void CPolygon::setVertex(const Vec2& point, int index)
{
	m_vertexs[index] = point;
}

Vec2 CPolygon::getVertex(int index)
{
	return m_vertexs[index];
}

void CPolygon::setAlpha(float alpha)
{
	m_alpha = alpha;
}

void CPolygon::debugDraw(DrawNode* pDrawNode)
{
	if (!m_isVisible || pDrawNode == NULL)
	{
		return;
	}

	if (m_count < 3) return;

	Color4F color = m_color;
	color.a *= m_alpha;

	Color4F fillColor = color;
	fillColor.a *= 0.5f;
	
	Vector2dVector vertexs;
	for (auto i = 0; i < m_count; ++i)
	{
		vertexs.push_back(m_vertexs[i]);
	}

	Vector2dVector result;
	//  Invoke the triangulator to triangulate this polygon.
	Triangulate::Process(vertexs, result);

	// print out the results.
	int tcount = result.size() / 3;

	for (int i = 0; i < tcount; i++)
	{
		pDrawNode->drawSolidPoly(&result[i * 3], 3, fillColor);
	}


	for (auto i = 0; i < m_count; ++i)
	{
		if (i == m_count - 1)
			pDrawNode->drawLine(m_vertexs[i], m_vertexs[0], color);
		else
			pDrawNode->drawLine(m_vertexs[i], m_vertexs[i + 1], color);
	}
}

void CPolygon::setVisible(bool value)
{
	m_isVisible = value;
}

void CPolygon::setColor(unsigned int color)
{
	float sc = 1.0f / 255.0f;
	m_color.r = (float)((color >> IM_COL32_R_SHIFT) & 0xFF) * sc;
	m_color.g = (float)((color >> IM_COL32_G_SHIFT) & 0xFF) * sc;
	m_color.b = (float)((color >> IM_COL32_B_SHIFT) & 0xFF) * sc;
	m_color.a = (float)((color >> IM_COL32_A_SHIFT) & 0xFF) * sc;

	m_color_Uint = color;
}