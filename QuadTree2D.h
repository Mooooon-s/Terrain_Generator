#ifndef __QUAD_TREE2D_H__

#pragma once
#include "Vec3.h"
#include "Vertex.h"
#include <vector>

using namespace std;

//쿼드 트리에는 노드가 필요
class QuadNode2D
{
public:
	int					_Depth;			//노드의 깊이를 가짐
	bool				_Leaf;				//잎노드인지 아닌지 판별
	Vec3<double>		_Min;
	Vec3<double>		_Max;
	vector<Vertex *>	_verteices;		//노드에는 파티클을 가지기 때문에 필요
	QuadNode2D			*_ChildNodes[4];	//노드는 자식노드 4개를 가짐
public:
	QuadNode2D(int depth);					//깊이에 따른 생성
	QuadNode2D(Vec3<double> min, Vec3<double> max);
	QuadNode2D(int depth, Vec3<double> min, Vec3<double> max);
	~QuadNode2D(void) {}
public:
	void	Subdivide(void);
	bool	IsEmpty(void);
	bool	IsContain(Vec3<double> p);
	bool	IsContain(Vec3<double> p, double r);
	bool	IsIntersect(Vec3<double> p, double r);
};

class QuadTree2D
{
public:
	int					_Bucket;			//몇 개 이상일 때 노드를 나눌 것인지
	int					_TargetDepth;		//전체 트리의 깊이
	QuadNode2D* _Root;
	vector<QuadNode2D*>	_DrawNodes;		//쿼드 노드를 저장 할수있는 공간 (그리기 용)
public:
	QuadTree2D();
	QuadTree2D(int depth, int bucket)
	{
		_TargetDepth = depth;
		_Bucket = bucket;
	}
	~QuadTree2D();
public:
	void		BuildTree(vector<Vertex*>& verteices);		//노드를 나누는 기능
	void		Query(vector<Vertex*>& verteices, double r, int index);
public:
	void		Darw(void);
};

#endif