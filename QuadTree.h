#ifndef __QUAD_TREE_H_

#pragma once
#include "Vertex.h"
#include "Vec3.h"
#include <vector>

using namespace std;

class QuadNode {
public:
	int					_depth;
	bool				_Leaf;
	Vec3<double>		_min;
	Vec3<double>		_max;
	vector<Vertex*>		_Vertex;
	QuadNode*			_ChildNodes[8];
public:
	QuadNode(int depth);
	QuadNode(Vec3<double> min, Vec3<double> max);
	QuadNode(int depth, Vec3<double> min, Vec3<double> max);
	~QuadNode(void);
public:
	void Subdivide(void);
	bool IsEmpty(void);
	bool Contain(Vec3<double> pos);
	bool Contain(Vec3<double> pos, double r);
	bool IsIntersect(Vec3<double> pos, double r);
};

class QuadTree {
public:
	int					_Bucket;
	int					_TargetDepth;
	QuadNode*			_Root;
	vector<QuadNode*>	_DrawNodes;
public:
	QuadTree();
	QuadTree(int TargetDepth, int bucket);
	~QuadTree();
public:
	void buildTree(vector<Vertex*>& vertex);
	void QueryTree(vector<Vertex*>& vertex, double r, int index);
	void Darw(void);
};
#endif