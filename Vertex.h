#ifndef __VERTEX_H__
#define __VERTEX_H__

#pragma once
#include "Vec3.h"
#include <vector>

using namespace std;

class Face;
class Vertex
{
public:
	int				_index;
	Vec3<float>		_fpos;
	Vec3<double>	_pos;
	Vec3<double>	_normal;
	vector<Face*>	_nbFaces; // nb(neighbor faces)
	bool			_Active;
	bool			_Collision;
	bool			_path;
	int				_pdepth=0;
public:
	Vertex();
	Vertex(int index, Vec3<double> pos)
	{
		_index = index;
		_pos = pos;
		_Active=false;
		_Collision=false;
		_path = false;
	}
	~Vertex();
public:
	inline double x(void) { return _pos[0]; }
	inline double y(void) { return _pos[1]; }
	inline double z(void) { return _pos[2]; }
};

#endif