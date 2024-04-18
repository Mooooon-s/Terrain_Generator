#include "QuadTree.h"
#include "GL/glut.h"

QuadNode::QuadNode(int depth)
{
	_Leaf = false;
	_depth = depth;
	for (int i = 0; i < 8; i++) {
		_ChildNodes[i] = nullptr;
	}
}

QuadNode::QuadNode(int depth, Vec3<double> min, Vec3<double> max)
{
	_Leaf = false;
	_depth = depth;
	_min = min;
	_max = max;
	for (int i = 0; i < 8; i++) {
		_ChildNodes[i] = nullptr;
	}
}

QuadNode::QuadNode(Vec3<double> min, Vec3<double> max) {
	_Leaf = false;
	_depth = 0;
	_min = min;
	_max = max;
	for (int i = 0; i < 8; i++) {
		_ChildNodes[i] = nullptr;
	}
}

QuadNode::~QuadNode()
{
}

QuadTree::QuadTree(int TargetDepth, int bucket)
{
	_TargetDepth = TargetDepth;
	_Bucket = bucket;
}

bool QuadNode::IsIntersect(Vec3<double> pos, double r)
{
	double dist = 0.0;
	for (int i = 0; i < 3; i++)
	{
		if (pos(i) < _min(i))
		{
			double d = _min(i) - pos(i);
			dist += d * d;
		}
		else if (pos(i) > _max(i)) {
			double d = pos(i) - _max(i);
			dist += d * d;
		}
	}
	return dist <= r * r;
}

bool QuadNode::Contain(Vec3<double> pos)
{
	if (pos.x() >= _min.x() && pos.x() <= _max.x() && pos.y() >= _min.y() && pos.y() <= _max.y() && pos.z() >= _min.z() && pos.z() <= _max.z())
	{
		return true;
	}
	return false;
}

bool QuadNode::Contain(Vec3<double> pos, double r) {
	double minx = pos.x() - r;
	double maxx = pos.x() + r;
	double miny = pos.y() - r;
	double maxy = pos.y() + r;
	double minz = pos.z() - r;
	double maxz = pos.z() + r;
	return (minx >= _min.x() && maxx < _max.x() && miny >= _min.y() && maxy < _max.y() && minz >= _min.z() && maxz < _max.z());
}

QuadTree::QuadTree()
{
}

QuadTree::~QuadTree()
{
}

void QuadNode::Subdivide() {
	for (int i = 0; i < 8; i++) {
		_ChildNodes[i] = new QuadNode(_depth + 1);
	}

	auto sumVec = _max + _min;
	double center_x = sumVec.x() / 2.0;
	double center_y = sumVec.y() / 2.0;
	double center_z = sumVec.z() / 2.0;

	//1층
//첫번째 자식 노드
	_ChildNodes[0]->_min.Set(_min.x(), _min.y(), _min.z());
	_ChildNodes[0]->_max.Set(center_x, center_y, center_z);
	//두번째 자식 노드
	_ChildNodes[1]->_min.Set(center_x, _min.y(), _min.z());
	_ChildNodes[1]->_max.Set(_max.x(), center_y, center_z);
	//세번째 자식 노드
	_ChildNodes[2]->_min.Set(_min.x(), _min.y(), center_z);
	_ChildNodes[2]->_max.Set(center_x, center_y, _max.z());
	//네번째 자식 노드
	_ChildNodes[3]->_min.Set(center_x, _min.y(), center_z);
	_ChildNodes[3]->_max.Set(_max.x(), center_y, _max.z());

	//2층
	//첫번째 자식 노드
	_ChildNodes[4]->_min.Set(_min.x(), center_y, _min.z());
	_ChildNodes[4]->_max.Set(center_x, _max.y(), center_z);
	//두번째 자식 노드
	_ChildNodes[5]->_min.Set(center_x, center_y, _min.z());
	_ChildNodes[5]->_max.Set(_max.x(), _max.y(), center_z);
	//세번째 자식 노드
	_ChildNodes[6]->_min.Set(_min.x(), center_y, center_z);
	_ChildNodes[6]->_max.Set(center_x, _max.y(), _max.z());
	//네번째 자식 노드
	_ChildNodes[7]->_min.Set(center_x, center_y, center_z);
	_ChildNodes[7]->_max.Set(_max.x(), _max.y(), _max.z());



	for (auto p : _Vertex)
	{

		for (int i = 0; i < 8; i++)
		{
			auto  childnode = _ChildNodes[i];
			if (childnode->Contain(p->_pos))
			{
				p->_path = true;
				p->_pdepth = _depth;
				childnode->_Vertex.push_back(p);
			}
		}
	}
	_Vertex.clear();
}

bool QuadNode::IsEmpty() {
	return _ChildNodes[0] == nullptr;
}

void QuadTree::buildTree(vector<Vertex*>& vertex)
{

	Vec3<double> min = Vec3<double>(vertex[0]->x(), vertex[0]->y(), vertex[0]->z());
	Vec3<double> max = Vec3<double>(vertex.back()->x(), vertex.back()->y()+3, vertex.back()->z());

	_Root = new QuadNode(1, min, max);
	for (auto p : vertex)
	{
		_Root->_Vertex.push_back(p);
	}

	vector<QuadNode*> queue;
	queue.push_back(_Root);

	while (queue.size() != 0)
	{
		auto node = queue[0];
		_DrawNodes.push_back(node);
		queue.erase(queue.begin());

		if (node->_depth< _TargetDepth && node->_Vertex.size()>_Bucket)
		{

			node->Subdivide();
			for (int i = 0; i < 8; i++)
			{
				queue.push_back(node->_ChildNodes[i]);
			}
		}

		if (node->_Vertex.size() > 0 && node->IsEmpty())
		{
			node->_Leaf = true;
		}

	}
	printf("QuadTree : build tree!\n");
}

void QuadTree::QueryTree(vector<Vertex*>& vertex, double r, int index)
{
	auto pos = vertex[index]->_pos;
	vector<QuadNode*> queue;
	queue.push_back(_Root);

	while (queue.size() != 0) {
		auto node = queue[0];
		queue.erase(queue.begin());
		
		bool contain = node->Contain(pos, r);
		bool intersect = node->IsIntersect(pos, r);
		if (contain == true || intersect == true)
		{
			if (node->_Leaf)
			{
				for (auto p : node->_Vertex) {
					p->_Active = true;
					double sdf = (p->_pos.x() - pos.x()) * (p->_pos.x() - pos.x()) + (p->_pos.y() - pos.y()) * (p->_pos.y() - pos.y()) + (p->_pos.z() - pos.z()) * (p->_pos.z() - pos.z()) - r * r;
					if (sdf <= 0.0) {
						p->_Collision = true;
					}
				}
			}
			if (node->IsEmpty() == false) {
				for (int i = 0; i < 8; i++) {
					queue.push_back(node->_ChildNodes[i]);
				}
			}
		}

	}
}

void QuadTree::Darw(void)
{
	glLineWidth(1.0);
	glDisable(GL_LIGHTING);
	for (auto n : _DrawNodes) {
		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		if (!n->_Leaf) {
			//continue;
		}
		glBegin(GL_LINES);
		glVertex3f(n->_min.x(), n->_min.y(), n->_min.z());//v1
		glVertex3f(n->_min.x(), n->_max.y(), n->_min.z());//v2

		glVertex3f(n->_min.x(), n->_max.y(), n->_min.z());//v2
		glVertex3f(n->_max.x(), n->_max.y(), n->_min.z());//v3

		glVertex3f(n->_max.x(), n->_max.y(), n->_min.z());//v3
		glVertex3f(n->_max.x(), n->_min.y(), n->_min.z());//v4

		glVertex3f(n->_max.x(), n->_min.y(), n->_min.z());//v4
		glVertex3f(n->_min.x(), n->_min.y(), n->_min.z());//v1

		glVertex3f(n->_min.x(), n->_min.y(), n->_max.z());//v5
		glVertex3f(n->_min.x(), n->_max.y(), n->_max.z());//v6

		glVertex3f(n->_min.x(), n->_max.y(), n->_max.z());//v6
		glVertex3f(n->_max.x(), n->_max.y(), n->_max.z());//v8

		glVertex3f(n->_max.x(), n->_max.y(), n->_max.z());//v8
		glVertex3f(n->_max.x(), n->_min.y(), n->_max.z());//v7

		glVertex3f(n->_min.x(), n->_min.y(), n->_max.z());//v5
		glVertex3f(n->_max.x(), n->_min.y(), n->_max.z());//v7

		glVertex3f(n->_min.x(), n->_min.y(), n->_min.z());//v1
		glVertex3f(n->_min.x(), n->_min.y(), n->_max.z());//v5

		glVertex3f(n->_min.x(), n->_max.y(), n->_max.z());//v6
		glVertex3f(n->_min.x(), n->_max.y(), n->_min.z());//v2

		glVertex3f(n->_max.x(), n->_min.y(), n->_max.z());//v7
		glVertex3f(n->_max.x(), n->_min.y(), n->_min.z());//v4

		glVertex3f(n->_max.x(), n->_max.y(), n->_max.z());//v8
		glVertex3f(n->_max.x(), n->_max.y(), n->_min.z());//v3

		glEnd();
		glPopMatrix();
	}
	glEnable(GL_LIGHTING);
	glLineWidth(1.0);
}

