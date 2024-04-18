#include "Mesh.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#define __QUAD_TREE2D_H__
//#define __KD_TREE_H_
#define __QUAD_TREE_H_

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
}

//load heightmap
void Mesh::LoadObj(char* obj) {
	int width, height, nChannels;
	int id = 0;
	_minBound.Set(100000000.0);
	_maxBound.Set(-100000000.0);

	// load height map texture
	unsigned char* data = stbi_load(obj, &width, &height, &nChannels, 0);
	Vec3<double> pos;
	double yScale = 64.0f / 256.0f, yShift = 16.0f;
	int rez = 1;
	unsigned bytePerPixel = nChannels;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
			unsigned char y = pixelOffset[0];
			
			// vertex
			pos[0] = ( (double)height * i / (double)height);   // vx
			//pos[1] = ((int)y * yScale);   // vy
			pos[1] = ((int)y);
			pos[2] =((double)width * j / (double)width);   // vz
			_vertices.push_back(new Vertex(id++, pos));
		}

		for (int i = 0; i < 3; i++) {
			if (_minBound[i] > pos[i]) _minBound[i] = pos[i];
			if (_maxBound[i] < pos[i]) _maxBound[i] = pos[i];
		}
	}
	std::cout << "Loaded " << _vertices.size() / 3 << " vertices" << std::endl;
	std::cout << height << " X " << width << std::endl;
	stbi_image_free(data);

	std::vector<unsigned> indices;
	for (unsigned i = 0; i < height - 1; i += rez)
	{
		for (unsigned j = 0; j < width; j += rez)
		{
			for (unsigned k = 0; k < 2; k++)
			{
				indices.push_back(j + width * (i + k * rez));
			}
		}
	}
	id = 0;
	double index[3];

	for (int i = 0; i < indices.size() - 3; i++)
	{
		if (i % 2 != 0) {//정 방향 삼각형
			index[0] = indices.at((i));
			index[1] = indices.at((i + 1));
			index[2] = indices.at((i + 2));
			_faces.push_back(new Face(id++, _vertices.at(index[0]), _vertices.at(index[1]), _vertices.at(index[2]), index[0], index[1], index[2]));//v1,v2,v3,t1,t2,t3
		}
		else {//역 방향 삼각형
			index[2] = indices.at((i));
			index[1] = indices.at((i + 1));
			index[0] = indices.at((i + 2));
			_faces.push_back(new Face(id++, _vertices.at(index[0]), _vertices.at(index[1]), _vertices.at(index[2]), index[0], index[1], index[2]));//v1,v2,v3,t1,t2,t3
		}
	}

	std::cout << "Loaded " << indices.size() << " indices" << std::endl;

	const int numStrips = (height - 1) / rez;
	const int numTrisPerStrip = (width / rez) * 2 - 2;
	std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
	std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;
	
	moveToCenter(10.0);
	makeAdjacencyList();
	computeNormal();
#ifdef __KD_TREE_H_
	KDTreeInit();
#endif // __KD_TREE_H_
#ifdef __QUAD_TREE_H_
	QuadTreeInit();
#endif // __QUAD_TREE_H_
#ifdef __QUAD_TREE2D_H__
	QuadTreeInit();
#endif // __QUAD_TREE2D_H__
}

void Mesh::makeAdjacencyList(void)
{
	// v-f
	for (auto f : _faces) {
		for (auto v : f->_vertices) {
			v->_nbFaces.push_back(f);
		}
	}
}

void Mesh::computeNormal(void)
{
	// face normal
	for (auto f : _faces) {
		auto v1 = f->v(1)->_pos - f->v(0)->_pos;
		auto v2 = f->v(2)->_pos - f->v(0)->_pos;
		f->_normal = v1.Cross(v2);
		f->_normal.Normalize();
	}

	// vertex normal
	for (auto v : _vertices) {
		for (auto nf : v->_nbFaces) {
			v->_normal += nf->_normal;
		}
		v->_normal /= (double)v->_nbFaces.size();
	}
}

void Mesh::moveToCenter(double scale)
{
	double crossLen = (_minBound - _maxBound).Length();
	auto center = (_maxBound + _minBound) / 2.0;
	Vec3<double> origin(0.0, 0.0, 0.0);

	for (auto v : _vertices) {
		auto pos = v->_pos;
		auto vec = pos - center;
		vec /= crossLen;
		vec *= scale;
		v->_pos = origin + vec;
	}
}

void Mesh::drawWire(void)
{
	glPushMatrix();
	glEnable(GL_LIGHTING);
	for (auto f : _faces) {
		glBegin(GL_LINES);
		for (int i = 0; i < 3; i++) {
			auto v0 = f->v((i + 1) % 3); // 2
			auto v1 = f->v((i + 2) % 3); // 0
			glNormal3f(v0->_normal.x(), v0->_normal.y(), v0->_normal.z());
			glVertex3f(v0->x(), v0->y(), v0->z());
			glNormal3f(v1->_normal.x(), v1->_normal.y(), v1->_normal.z());
			glVertex3f(v1->x(), v1->y(), v1->z());
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::drawSolid(bool smoothing)
{
	if (_Draw) {
#ifdef __KD_TREE_H_
		_KdTree->Darw();
#endif // __KD_TREE_H_
#ifdef __QUAD_TREE_H_
		_QuadTree->Darw();
#endif // __QUAD_TREE_H_
	}
	glPushMatrix();
	glEnable(GL_LIGHTING);
	smoothing == true ? glEnable(GL_SMOOTH) : glEnable(GL_FLAT);
	glEnable(GL_SMOOTH);
	for (auto f : _faces) {
		glBegin(GL_POLYGON);
		if (!smoothing) {
			glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
		}
		for (auto v : f->_vertices) {
			if (smoothing) {
				glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
			}
			glVertex3f(v->x(), v->y(), v->z());
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	glPopMatrix();
}


void Mesh::drawSolidWithPing(bool smoothing)
{
	if (_Draw) {
#ifdef __KD_TREE_H_
		_KdTree->Darw();
#endif // __KD_TREE_H_
#ifdef __QUAD_TREE_H_
		_QuadTree->Darw();
#endif // __QUAD_TREE_H_
#ifdef __QUAD_TREE2D_H__
		_QuadTree2D->Darw();
#endif // __QUAD_TREE2D_H_

}
	glPushMatrix();
	glEnable(GL_LIGHTING);
	smoothing == true ? glEnable(GL_SMOOTH) : glEnable(GL_FLAT);
	glEnable(GL_SMOOTH);
	for (auto f : _faces) {
		glBegin(GL_POLYGON);
		if (!smoothing) {
			glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
		}
		for (auto v : f->_vertices) {
			if (smoothing) {
				glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
			}
			glVertex3f(v->x(), v->y(), v->z());
		}
		glEnd();
	}
	drawPing();
	glDisable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::drawPoint(void)
{
	if(_Draw)
#ifdef __KD_TREE_H_
	_KdTree->Darw();
#endif // __KD_TREE_H_
#ifdef __QUAD_TREE_H_
	_QuadTree->Darw();
#endif // __QUAD_TREE_H_
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glPointSize(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	for (auto v : _vertices) {
		glVertex3f(v->x(), v->y(), v->z());
		if (v->y() == 0.0f)
		{
			glColor3f(0.0f, 0.0f, 0.5f);
		}
		else if (v->y() > 0.0f && v->y() < 0.1f) {
			glColor3f(0.5f, 1.0f, 0.0f);
		}
		else if(v->y() > 0.1f && v->y() < 0.3f)
		{
			glColor3f(0.0f, 1.0f, 0.0f);
		}
		else if (v->y() > 0.3f && v->y() < 0.6f)
		{
			glColor3f(0.0f, 0.8f, 0.0f);
		}
		else if (v->y() > 0.6f && v->y() < 0.9f)
		{
			glColor3f(0.0f, 0.6f, 0.0f);
		}
		else if (v->y() > 0.9f && v->y() < 1.2f)
		{
			glColor3f(0.0f, 0.4f, 0.0f);
		}
		else if (v->y() > 1.2f && v->y() < 1.5f)
		{
			glColor3f(0.0f, 0.2f, 0.0f);
		}
		else if (v->y() > 1.5f && v->y() < 1.8f)
		{
			glColor3f(0.0f, 0.1f, 0.0f);
		}
		else {
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		if (v->_Active == true) {
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		if (v->_Collision == true) {
			glColor3f(1.0f, 1.0f, 0.0f);
		}
	}
	glEnd();
	glPointSize(1.0f);
	glPopMatrix();
}

void Mesh::drawPing() {
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glPointSize(5.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);

	for (auto v : _vertices) {
#ifdef __QUAD_TREE_H_
		path(v);
#endif // __QUAD_TREE_H_
#ifdef __KD_TREE_H_
		path(v);
#endif // __KD_TREE_H_
#ifdef __QUAD_TREE2D_H__
		path(v);
#endif // __QUAD_TREE2D_H_
		if (v->_Collision) {
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(v->x(), v->y(), v->z());
		}
		if (v->_Active) {
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(v->x(), v->y(), v->z());
		}
	}
	glEnd();
	glPointSize(1.0f);
	glPopMatrix();
	arrow(_vertices, _pingindex);
}

void Mesh::path(Vertex* v) {
	if (v->_path) {
		switch (v->_pdepth)
		{
		case 0:
			glColor3f(0.0f, 1.0f, 0.1f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 1:
			glColor3f(0.0f, 0.9f, 0.2f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 2:
			glColor3f(0.0f, 0.8f, 0.3f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 3:
			glColor3f(0.0f, 0.7f, 0.4f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 4:
			glColor3f(0.0f, 0.6f, 0.5f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 5:
			glColor3f(0.0f, 0.5f, 0.6f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 6:
			glColor3f(0.0f, 0.4f, 0.7f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 7:
			glColor3f(0.0f, 0.3f, 0.8f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 8:
			glColor3f(0.0f, 0.2f, 0.9f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		case 9:
			glColor3f(0.0f, 0.1f, 1.0f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		default:
			glColor3f(1.0f, 0.2f, 0.9f);
			glVertex3f(v->x(), v->y(), v->z());
			break;
		}
	}
}

void Mesh::KDTreeInit() {
	_KdTree = new KdTree(1000);
	_KdTree->BuildTree(_vertices);
	_KdTree->Query(_vertices, 0.08,33);
	_pingindex = 33;
}

void Mesh::KDRQuery() {
	int idx = (rand() * 100) % _vertices.size();
	_KdTree->Query(_vertices, 0.08,idx );
	_pingindex = idx;
}

void	Mesh::InitFlag() {
	for (auto v : _vertices) {
		v->_Active = false;
		v->_Collision = false;
		v->_path = false;
		v->_pdepth = 0;
	}
}

void Mesh::QuadTreeInit() {
#ifdef __QUAD_TREE2D_H__
	_QuadTree2D = new QuadTree2D(7, 1000);	//depth,buket
	_QuadTree2D->BuildTree(_vertices);
	_QuadTree2D->Query(_vertices, 0.08, 33);
	_pingindex = 33;
#endif // __QUAD_TREE2D_H__
#ifdef __QUAD_TREE_H_
	_QuadTree = new QuadTree(7, 1000);	//depth,buket
	_QuadTree->buildTree(_vertices);
	_QuadTree->QueryTree(_vertices, 0.08, 33);
	_pingindex = 33;
#endif // __QUAD_TREE2D_H__
}

void Mesh::QuadRQuery() {
	
#ifdef __QUAD_TREE_H_
	auto idx = (rand() * 100) % _vertices.size();
	_pingindex = idx;
	_QuadTree->QueryTree(_vertices, 0.08, idx);
#endif // __QUAD_TREE_H_
#ifdef __QUAD_TREE2D_H__
	 auto idx = (rand() * 100) % _vertices.size();
	 _pingindex = idx;
	_QuadTree2D->Query(_vertices, 0.08, idx);
#endif // __QUAD_TREE2D_H_
}

void Mesh::Ctrl() {
#ifdef __KD_TREE_H_
	InitFlag();
	KDRQuery();
#endif // __KD_TREE_H_
#ifdef __QUAD_TREE_H_
	InitFlag();
	QuadRQuery();
#endif // __QUAD_TREE_H_
#ifdef __QUAD_TREE2D_H__
	InitFlag();
	QuadRQuery();
#endif // __QUAD_TREE2D_H_
}

void Mesh::arrow(vector<Vertex*>& v,int index) {
	Vec3<double> s = v[index]->_pos;
	Vec3<double> norm;
	for (auto a : v) {
		if (a->_Active == true) {
			for (auto nf : a->_nbFaces) {
				norm += nf->_normal;
			}
		}
	}
	norm.Normalize();
	Vec3<double> e = s + norm;
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(5.0);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glVertex3f(s.x(), s.y(), s.z());
	glVertex3f(e.x(), e.y(), e.z());
	glEnd();
	glEnable(GL_LIGHTING);
}

