#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "Face.h"
#include "Texture.h"
#include "GL/glut.h"
#include "KdTree.h"
#include "QuadTree.h"
#include "QuadTree2D.h"

class Mesh
{
public:
	vector<Face*>		_faces;
	vector<Vertex*>		_vertices;
	vector<Texture*>	_texturecoord;
	GLuint				_textureindex;
	Vec3<double>		_minBound;
	Vec3<double>		_maxBound;
	int					_pingindex;
	KdTree				*_KdTree;
	QuadTree			*_QuadTree;
	QuadTree2D			* _QuadTree2D;
	bool				_Draw=true;
public:
	Mesh();
	Mesh(char * obj,char* texture)
	{
		LoadObj(obj);
	}
	~Mesh();
public:
	void	LoadObj(char* obj);
	void	computeNormal(void);
	void	makeAdjacencyList(void);
	void	moveToCenter(double scale = 1.0);
public:
	void	drawWire(void);
	void	drawPoint(void);
	void	drawSolid(bool smoothing);
	void	drawSolidWithPing(bool smoothing);
	void	drawPing(void);
	void	arrow(vector<Vertex*>& v,int index);
	void	path(Vertex* v);
public:
	void	KDTreeInit();
	void	KDRQuery();
public:
	void	QuadTreeInit();
	void	QuadRQuery();
public:
	void	InitFlag();
	void	Ctrl();

};

#endif