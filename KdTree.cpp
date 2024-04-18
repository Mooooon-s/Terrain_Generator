#include "KdTree.h"
#include "GL\glut.h"

KdNode::KdNode(void)
{
	for (int i = 0; i < 2; i++) {
		_ChildNodes[i] = nullptr;
	}
}

KdNode::KdNode(Vec3<double> min, Vec3<double> max)
{
	_Min = min;
	_Max = max;
	for (int i = 0; i < 2; i++) {
		_ChildNodes[i] = nullptr;
	}
}

KdNode::KdNode(int depth, Vec3<double> min, Vec3<double> max)
{
	_Min = min;
	_Max = max;
	for (int i = 0; i < 2; i++) {
		_ChildNodes[i] = nullptr;
	}
}

bool KdNode::IsContain(Vec3<double> p)
{
	if (p.x() >= _Min.x() && p.x() <= _Max.x() && p.y() >= _Min.y() && p.y() <= _Max.y() && p.z() >= _Min.z() && p.z() <= _Max.z()) {
		return true;
	}
	return false;
}

bool KdNode::IsIntersect(Vec3<double> p, double r)
{
	double dist = 0.0;
	for (int i = 0; i < 3; i++) {
		if (p(i) < _Min(i)) {
			double d = _Min(i) - p(i);
			dist += d * d;
		}
		else if (p(i) > _Max(i)) {
			double d = p(i) - _Max(i);
			dist += d * d;
		}
	}
	return dist <= r * r;
}

bool KdNode::IsEmpty(void)
{
	return _ChildNodes[0] == nullptr;
}

bool KdNode::IsContain(Vec3<double> p, double r)
{
	double minx = p.x() - r;
	double maxx = p.x() + r;
	double miny = p.y() - r;
	double maxy = p.y() + r;
	double minz = p.z() - r;
	double maxz = p.z() + r;
	return (minx >= _Min.x() && maxx <= _Max.x() && miny >= _Min.y() && maxy <= _Max.y() && minz >= _Min.z() && maxz <= _Max.z());
}

void KdNode::Subdivide(void)
{
	for (int i = 0; i < 2; i++) {
		_ChildNodes[i] = new KdNode();
	}

	auto corssVec = _Max - _Min;
	int mid = _Vertex.size() / 2;
	double Axisyz = corssVec.y() > corssVec.z() ? corssVec.y() : corssVec.z();
	double AxisxA = corssVec.x() > Axisyz ? corssVec.x() : Axisyz;
	int splitAxis;

	if (AxisxA == corssVec.x())
		splitAxis = 0;
	else if (AxisxA == corssVec.y())
		splitAxis = 1;
	else if (AxisxA == corssVec.z())
		splitAxis = 2;

	nth_element(_Vertex.begin(), _Vertex.begin() + mid, _Vertex.end(), [splitAxis](Vertex* a, Vertex* b) {return a->_pos(splitAxis) < b->_pos(splitAxis); });//stl ±â´É

	auto separation = _Vertex[mid]->_pos(splitAxis);
	auto localMin = _Min;
	auto localMax = _Max;
	localMin(splitAxis) = separation;
	localMax(splitAxis) = separation;

	_ChildNodes[0]->_Min = _Min;
	_ChildNodes[0]->_Max = localMax;
	_ChildNodes[1]->_Min = localMin;
	_ChildNodes[1]->_Max = _Max;

	// Transfer particles
	_ChildNodes[0]->_Vertex.clear();
	_ChildNodes[0]->_Vertex.assign(_Vertex.begin(), _Vertex.begin() + mid);
	_ChildNodes[1]->_Vertex.clear();
	_ChildNodes[1]->_Vertex.assign(_Vertex.begin() + mid, _Vertex.end());

	for (auto v : _ChildNodes[0]->_Vertex) {
		v->_path = true;
		v->_pdepth++;
	}

	_Vertex.clear();
}

KdTree::KdTree()
{
}

KdTree::~KdTree()
{
}

void KdTree::BuildTree(vector<Vertex*>& vertices)
{
	Vec3<double> min = Vec3<double>(vertices[0]->x(), vertices[0]->y(), vertices[0]->z());
	Vec3<double> max = Vec3<double>(vertices.back()->x(), vertices.back()->y()+3, vertices.back()->z());
	_Root = new KdNode(1, min ,max);
	printf("%d\n",vertices.size());
	// Transfer particles
	for (auto p : vertices) {
		_Root->_Vertex.push_back(p);
	}

	vector<KdNode*> queue;
	queue.push_back(_Root);

	int debug = 0;

	while (queue.size() != 0) {
		auto node = queue[0];
		_DrawNodes.push_back(node);
		queue.erase(queue.begin());
		if (node->_Vertex.size() > _Bucket) {
			node->Subdivide();
			for (int i = 0; i < 2; i++) {
				queue.push_back(node->_ChildNodes[i]);
			}
		}
	}
	printf("KdTree : build tree!\n");
}

void KdTree::Query(vector<Vertex*>& vertices, double r, int index)
{
	auto pos = vertices[index]->_pos;
	vector<KdNode*> queue;
	queue.push_back(_Root);

	while (queue.size() != 0) {
		auto node = queue[0];
		queue.erase(queue.begin());

		bool contain = node->IsContain(pos, r);
		bool intersect = node->IsIntersect(pos, r);
		if (contain == true || intersect == true) {
			for (auto p : node->_Vertex) {
				p->_Active = true;
				double sdf = (p->_pos.x() - pos.x()) * (p->_pos.x() - pos.x()) + (p->_pos.y() - pos.y()) * (p->_pos.y() - pos.y()) + (p->_pos.z() - pos.z()) * (p->_pos.z() - pos.z()) - r * r;
				if (sdf <= 0.0) {
					p->_Collision = true;
				}
			}
			if (node->IsEmpty() == false) {
				for (int i = 0; i < 2; i++) {
					queue.push_back(node->_ChildNodes[i]);
				}
			}
		}

	}


}

void KdTree::Darw(void)
{
	glLineWidth(1.0);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	for (auto n : _DrawNodes) {
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(n->_Min.x(), n->_Min.y(), n->_Min.z());//v1
		glVertex3f(n->_Min.x(), n->_Max.y(), n->_Min.z());//v2

		glVertex3f(n->_Min.x(), n->_Max.y(), n->_Min.z());//v2
		glVertex3f(n->_Max.x(), n->_Max.y(), n->_Min.z());//v3

		glVertex3f(n->_Max.x(), n->_Max.y(), n->_Min.z());//v3
		glVertex3f(n->_Max.x(), n->_Min.y(), n->_Min.z());//v4

		glVertex3f(n->_Max.x(), n->_Min.y(), n->_Min.z());//v4
		glVertex3f(n->_Min.x(), n->_Min.y(), n->_Min.z());//v1

		glVertex3f(n->_Min.x(), n->_Min.y(), n->_Max.z());//v5
		glVertex3f(n->_Min.x(), n->_Max.y(), n->_Max.z());//v6

		glVertex3f(n->_Min.x(), n->_Max.y(), n->_Max.z());//v6
		glVertex3f(n->_Max.x(), n->_Max.y(), n->_Max.z());//v8

		glVertex3f(n->_Max.x(), n->_Max.y(), n->_Max.z());//v8
		glVertex3f(n->_Max.x(), n->_Min.y(), n->_Max.z());//v7

		glVertex3f(n->_Min.x(), n->_Min.y(), n->_Max.z());//v5
		glVertex3f(n->_Max.x(), n->_Min.y(), n->_Max.z());//v7

		glVertex3f(n->_Min.x(), n->_Min.y(), n->_Min.z());//v1
		glVertex3f(n->_Min.x(), n->_Min.y(), n->_Max.z());//v5

		glVertex3f(n->_Min.x(), n->_Max.y(), n->_Max.z());//v6
		glVertex3f(n->_Min.x(), n->_Max.y(), n->_Min.z());//v2

		glVertex3f(n->_Max.x(), n->_Min.y(), n->_Max.z());//v7
		glVertex3f(n->_Max.x(), n->_Min.y(), n->_Min.z());//v4

		glVertex3f(n->_Max.x(), n->_Max.y(), n->_Max.z());//v8
		glVertex3f(n->_Max.x(), n->_Max.y(), n->_Min.z());//v3
		glEnd();
		glPopMatrix();
	}
	glEnable(GL_LIGHTING);
	glLineWidth(1.0);
}