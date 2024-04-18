#include "QuadTree2D.h"
#include "GL\glut.h"

QuadNode2D::QuadNode2D(int depth)
{
	_Leaf = false;						//�ٳ�� �ʱ�ȭ
	_Depth = depth;						//���� ����
	for (int i = 0; i < 4; i++) {		//�ڽĳ�� �ʱ�ȭ
		_ChildNodes[i] = nullptr;
	}
}

QuadNode2D::QuadNode2D(Vec3<double> min, Vec3<double> max)//�ּ� �ִ밪�� �־��� �� ��� ����
{
	_Leaf = false;
	_Depth = 0;					//���̰� ����
	_Min = min;
	_Max = max;
	for (int i = 0; i < 4; i++) {
		_ChildNodes[i] = nullptr;
	}
}

QuadNode2D::QuadNode2D(int depth, Vec3<double> min, Vec3<double> max)	//��ġ�� ���̰� ���� �־����� ��
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
	//�ش� ����� ���� �ȿ� ��ġ���ִٸ� ��
	if (p.x() >= _Min.x() && p.x() <= _Max.x() && p.z() >= _Min.z() && p.z() <= _Max.z()) {
		return true;
	}
	return false;
}

void QuadNode2D::Subdivide(void)
{
	//�ڽ� ��带 ����
	for (int i = 0; i < 4; i++) {
		_ChildNodes[i] = new QuadNode2D(_Depth + 1);	//���̸� �÷���
	}
	//�簢���� �׸��� ���� ����
	auto sumVec = _Max + _Min;
	double center_x = sumVec.x() / 2.0;	//�߰� x
	double center_z = sumVec.z() / 2.0;	//�߰� z

	//ù��° �ڽ� ���
	_ChildNodes[0]->_Min.Set(_Min.x(), 0.0, center_z);
	_ChildNodes[0]->_Max.Set(center_x,  0.0, _Max.z());
	//�ι�° �ڽ� ���
	_ChildNodes[1]->_Min.Set(center_x,  0.0, center_z);
	_ChildNodes[1]->_Max.Set(_Max.x(),  0.0, _Max.z());
	//����° �ڽ� ���
	_ChildNodes[2]->_Min.Set(_Min.x(),  0.0, _Min.z());
	_ChildNodes[2]->_Max.Set(center_x, 0.0, center_z);
	//�׹�° �ڽ� ���
	_ChildNodes[3]->_Min.Set(center_x, 0.0, _Min.z());
	_ChildNodes[3]->_Max.Set(_Max.x(), 0.0, center_z);

	// Transfer particles ��ƼŬ�� �ڽĳ��� �ű�
	for (auto p : _verteices) {
		for (int i = 0; i < 4; i++) {
			auto childNode = _ChildNodes[i];
			if (childNode->IsContain(p->_pos)) {	//���ȿ� �ִ��� ������ �Ǻ�
				p->_path = true;
				p->_pdepth = _Depth;
				childNode->_verteices.push_back(p);//�ڽĳ�忡 ��ƼŬ�� ����
			}
		}
	}

	_verteices.clear();//�ڽĳ�忡 �Ѱ���� ������ ������忡 �ִ� ��ƼŬ�� ����
}

void QuadTree2D::BuildTree(vector<Vertex*>& vertices)
{
	Vec3<double> min = Vec3<double>(vertices[0]->x(), 0, vertices[0]->z());
	Vec3<double> max = Vec3<double>(vertices.back()->x(), 0, vertices.back()->z());
	_Root = new QuadNode2D(1, min, max);	//���̴� 1, min,max ���� �ε� ��ƼŬ�� 0 �̳� 1�� ���� �ɼ��� �ֱ⿡ ������ ���� ����

	// Transfer particles
	//ù��° ���� ������ ��� ��ƼŬ�� ��Ʈ ��忡 ����
	for (auto p : vertices) {
		_Root->_verteices.push_back(p);
	}

	vector<QuadNode2D*> queue;
	queue.push_back(_Root);

	while (queue.size() != 0) {
		auto node = queue[0];
		_DrawNodes.push_back(node); //������ ��
		queue.erase(queue.begin());	//ù��° ��� pop

		//�������ΰ� �ȳ��� ���ΰ� 
		//(���� ���̰� Ʈ�� ���̺��� �۰� ��忡 ��ƼŬ ������ ���� ���غ��� ū��)
		if (node->_Depth < _TargetDepth && node->_verteices.size() > _Bucket) {
			//if (node->m_Particles.size() > m_Bucket) {
			node->Subdivide();
			for (int i = 0; i < 4; i++) {
				queue.push_back(node->_ChildNodes[i]);
			}
		}

		//�ڽĳ�尡 ���� ��ƼŬ�� �������ִٸ� �� ���
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