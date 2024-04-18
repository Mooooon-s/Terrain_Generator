#include "QuadTree2D.h"
#include "GL\glut.h"

QuadNode2D::QuadNode2D(int depth)
{
	_Leaf = false;						//잎노드 초기화
	_Depth = depth;						//깊이 설정
	for (int i = 0; i < 4; i++) {		//자식노드 초기화
		_ChildNodes[i] = nullptr;
	}
}

QuadNode2D::QuadNode2D(Vec3<double> min, Vec3<double> max)//최소 최대값을 주었을 때 노드 생성
{
	_Leaf = false;
	_Depth = 0;					//깊이가 없음
	_Min = min;
	_Max = max;
	for (int i = 0; i < 4; i++) {
		_ChildNodes[i] = nullptr;
	}
}

QuadNode2D::QuadNode2D(int depth, Vec3<double> min, Vec3<double> max)	//위치와 깊이가 같이 주어졌을 때
{
	_Leaf = false;
	_Depth = depth;
	_Min = min;
	_Max = max;
	for (int i = 0; i < 4; i++) {
		_ChildNodes[i] = nullptr;
	}
}

bool QuadNode2D::IsIntersect(Vec3<double> p, double r)
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

bool QuadNode2D::IsEmpty(void)
{
	return _ChildNodes[0] == nullptr;
}

bool QuadNode2D::IsContain(Vec3<double> p, double r)
{
	double minx = p.x() - r;
	double maxx = p.x() + r;
	double minz = p.z() - r;
	double maxz = p.z() + r;
	return (minx >= _Min.x() && maxx < _Max.x() && minz >= _Min.z() && maxz < _Max.z());
}

QuadTree2D::QuadTree2D()
{
}

QuadTree2D::~QuadTree2D()
{
}

void QuadTree2D::Darw(void)
{
	glLineWidth(1.0);
	glDisable(GL_LIGHTING);
	for (auto n : _DrawNodes) {
		glPushMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		if (!n->_Leaf) {
			//continue;
		}
		glBegin(GL_LINES);

		glVertex3f(n->_Min.x(),0, n->_Min.z());
		glVertex3f(n->_Min.x(),0, n->_Max.z());

		glVertex3f(n->_Min.x(),0, n->_Max.z());
		glVertex3f(n->_Max.x(),0, n->_Max.z());

		glVertex3f(n->_Max.x(),0, n->_Max.z());
		glVertex3f(n->_Max.x(),0, n->_Min.z());

		glVertex3f(n->_Max.x(),0, n->_Min.z());
		glVertex3f(n->_Min.x(),0, n->_Min.z());

		glEnd();
		glPopMatrix();
	}
	glEnable(GL_LIGHTING);
	glLineWidth(1.0);
}

bool QuadNode2D::IsContain(Vec3<double> p)
{
	//해당 노드의 범위 안에 위치해있다면 참
	if (p.x() >= _Min.x() && p.x() <= _Max.x() && p.z() >= _Min.z() && p.z() <= _Max.z()) {
		return true;
	}
	return false;
}

void QuadNode2D::Subdivide(void)
{
	//자식 노드를 생성
	for (int i = 0; i < 4; i++) {
		_ChildNodes[i] = new QuadNode2D(_Depth + 1);	//깊이를 늘려줌
	}
	//사각형을 그리기 위한 과정
	auto sumVec = _Max + _Min;
	double center_x = sumVec.x() / 2.0;	//중간 x
	double center_z = sumVec.z() / 2.0;	//중간 z

	//첫번째 자식 노드
	_ChildNodes[0]->_Min.Set(_Min.x(), 0.0, center_z);
	_ChildNodes[0]->_Max.Set(center_x,  0.0, _Max.z());
	//두번째 자식 노드
	_ChildNodes[1]->_Min.Set(center_x,  0.0, center_z);
	_ChildNodes[1]->_Max.Set(_Max.x(),  0.0, _Max.z());
	//세번째 자식 노드
	_ChildNodes[2]->_Min.Set(_Min.x(),  0.0, _Min.z());
	_ChildNodes[2]->_Max.Set(center_x, 0.0, center_z);
	//네번째 자식 노드
	_ChildNodes[3]->_Min.Set(center_x, 0.0, _Min.z());
	_ChildNodes[3]->_Max.Set(_Max.x(), 0.0, center_z);

	// Transfer particles 파티클을 자식노드로 옮김
	for (auto p : _verteices) {
		for (int i = 0; i < 4; i++) {
			auto childNode = _ChildNodes[i];
			if (childNode->IsContain(p->_pos)) {	//노드안에 있는지 없는지 판별
				p->_path = true;
				p->_pdepth = _Depth;
				childNode->_verteices.push_back(p);//자식노드에 파티클을 저장
			}
		}
	}

	_verteices.clear();//자식노드에 넘겨줬기 때문에 상위노드에 있는 파티클을 제거
}

void QuadTree2D::BuildTree(vector<Vertex*>& vertices)
{
	Vec3<double> min = Vec3<double>(vertices[0]->x(), 0, vertices[0]->z());
	Vec3<double> max = Vec3<double>(vertices.back()->x(), 0, vertices.back()->z());
	_Root = new QuadNode2D(1, min, max);	//깊이는 1, min,max 범위 인데 파티클이 0 이나 1에 생성 될수도 있기에 범위를 조금 넓힘

	// Transfer particles
	//첫번째 깊이 에서는 모든 파티클이 루트 노드에 존재
	for (auto p : vertices) {
		_Root->_verteices.push_back(p);
	}

	vector<QuadNode2D*> queue;
	queue.push_back(_Root);

	while (queue.size() != 0) {
		auto node = queue[0];
		_DrawNodes.push_back(node); //랜더링 용
		queue.erase(queue.begin());	//첫번째 노드 pop

		//나눌것인가 안나눌 것인가 
		//(현재 깊이가 트리 깊이보다 작고 노드에 파티클 갯수가 나눌 기준보다 큰가)
		if (node->_Depth < _TargetDepth && node->_verteices.size() > _Bucket) {
			//if (node->m_Particles.size() > m_Bucket) {
			node->Subdivide();
			for (int i = 0; i < 4; i++) {
				queue.push_back(node->_ChildNodes[i]);
			}
		}

		//자식노드가 없고 파티클을 가지고있다면 잎 노드
		if (node->_verteices.size() > 0 && node->IsEmpty()) {
			node->_Leaf = true;
		}
	}
	printf("QuadTree : build tree!\n");
}

void QuadTree2D::Query(vector<Vertex*>& particles, double r, int index)
{
	auto pos = Vec3<double>( particles[index]->_pos.x(),0, particles[index]->_pos.z());
	vector<QuadNode2D*> queue;
	queue.push_back(_Root);

	while (queue.size() != 0) {
		auto node = queue[0];
		queue.erase(queue.begin());
		bool contain = node->IsContain(pos, r);
		bool intersect = node->IsIntersect(pos, r);
		if (contain == true || intersect == true) {
			if (node->_Leaf) {
				for (auto p : node->_verteices) {
					p->_Active = true;
					double sdf = (p->_pos.x() - pos.x()) * (p->_pos.x() - pos.x()) + (p->_pos.z() - pos.z()) * (p->_pos.z() - pos.z()) - r * r;
					if (sdf <= 0.0) {
						p->_Collision = true;
					}
				}
			}
			if (node->IsEmpty() == false) {
				for (int i = 0; i < 4; i++) {
					queue.push_back(node->_ChildNodes[i]);
				}
			}
		}
	}
}