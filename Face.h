#ifndef __FACE_H__
#define __FACE_H__

#pragma once
#include "Vertex.h"
#include <vector>

using namespace std;

class Face
{
public:
	int				_index;
	int				_texelpos[3];
	Vec3<double>	_normal;
	vector<Vertex*>	_vertices;
public:
	Face();
	Face(int index, Vertex *v0, Vertex * v1, Vertex *v2)
	{
		_index = index;
		_vertices.push_back(v0);
		_vertices.push_back(v1);
		_vertices.push_back(v2);
	}
	~Face();
	Face(int index, Vertex* v0, Vertex* v1, Vertex* v2,int tu,int tv,int tw)
	{
		_index = index;
		_vertices.push_back(v0);
		_vertices.push_back(v1);
		_vertices.push_back(v2);
		_texelpos[0] = tu;
		_texelpos[1] = tv;
		_texelpos[2] = tw;
	}

public:
	inline Vertex *v(int index) { return _vertices[index]; }
};
#endif

