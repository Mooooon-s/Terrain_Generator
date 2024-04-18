#ifndef __QUAD_TREE2D_H__

#pragma once
#include "Vec3.h"
#include "Vertex.h"
#include <vector>

using namespace std;

//���� Ʈ������ ��尡 �ʿ�
class QuadNode2D
{
public:
	int					_Depth;			//����� ���̸� ����
	bool				_Leaf;				//�ٳ������ �ƴ��� �Ǻ�
	Vec3<double>		_Min;
	Vec3<double>		_Max;
	vector<Vertex *>	_verteices;		//��忡�� ��ƼŬ�� ������ ������ �ʿ�
	QuadNode2D			*_ChildNodes[4];	//���� �ڽĳ�� 4���� ����
public:
	QuadNode2D(int depth);					//���̿� ���� ����
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
	int					_Bucket;			//�� �� �̻��� �� ��带 ���� ������
	int					_TargetDepth;		//��ü Ʈ���� ����
	QuadNode2D* _Root;
	vector<QuadNode2D*>	_DrawNodes;		//���� ��带 ���� �Ҽ��ִ� ���� (�׸��� ��)
public:
	QuadTree2D();
	QuadTree2D(int depth, int bucket)
	{
		_TargetDepth = depth;
		_Bucket = bucket;
	}
	~QuadTree2D();
public:
	void		BuildTree(vector<Vertex*>& verteices);		//��带 ������ ���
	void		Query(vector<Vertex*>& verteices, double r, int index);
public:
	void		Darw(void);
};

#endif