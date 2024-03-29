#pragma once

// ref https://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml

#include "cocos2d.h"

USING_NS_CC;

// Typedef an STL vector of vertices which are used to represent
// a polygon/contour and a series of triangles.
typedef std::vector< Vec2> Vector2dVector;


class Triangulate
{
public:

	// triangulate a contour/polygon, places results in STL vector
	// as series of triangles.
	static bool Process(const Vector2dVector &contour,
		Vector2dVector &result);

	// compute area of a contour/polygon
	static float Area(const Vector2dVector &contour);

	// decide if point Px/Py is inside triangle defined by
	// (Ax,Ay) (Bx,By) (Cx,Cy)
	static bool InsideTriangle(float Ax, float Ay,
		float Bx, float By,
		float Cx, float Cy,
		float Px, float Py);


private:
	static bool Snip(const Vector2dVector &contour, int u, int v, int w, int n, int *V);

};
