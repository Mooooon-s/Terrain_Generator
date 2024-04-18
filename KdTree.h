#ifndef __KD_TREE_H_

#pragma once
#include "Vec3.h"
#include<vector>
#include "Vertex.h"
#include <vector>
#include <algorithm>

using namespace std;

class KdNode
{
public:
	Vec3<double>		_Min;
	Vec3<double>		_Max;
	vector<Vertex*>		_Vertex;
	KdNode				*_ChildNodes[2];
public:
	KdNode(void);
	KdNode(Vec3<double> min, Vec3<double> max);
	KdNode(int depth, Vec3<double> min, Vec3<double> max);
	~KdNode(void) {}
public:
	inline Vec3<double> GetCenter(void) { return (_Max + _Min) / 2.0; }
public:
	bool	IsEmpty(void);
	bool	IsContain(Vec3<double> p);
	bool	IsContain(Vec3<double> p, double r);
	bool	IsIntersect(Vec3<double> p, double r);
	void	Subdivide(void);
public:
};

class KdTree
{
public:
	int				_Bucket;
	KdNode*			_Root;
	vector<KdNode*>	_DrawNodes;
public:
	KdTree();
	KdTree(int bucket)
	{
		_Bucket = bucket;
	}
	~KdTree();
public:
	void	BuildTree(vector<Vertex*>& verteices);
	void	Query(vector<Vertex*>& verteices, double r, int index);
public:
	void	Darw(void);
};

#endif // !__KD_TREE_H_