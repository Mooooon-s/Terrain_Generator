#include <stdio.h>
#include <Windows.h>
#include "GL\glut.h"
#include "Mesh.h"
#include "Vertex.h"

float			_zoom = 15.0f;
float			_rotate_x = 0.0f;
float			_rotate_y = 0.001f;
float			_translate_x = 0.0f;
float			_translate_y = 0.0f;
int				last_x = 0;
int				last_y = 0;
unsigned char	_btnStates[3] = { 0 };
bool			_smoothing = false;
Mesh			*_mesh;

 

void init(void)
{
	glEnable(GL_DEPTH_TEST);
}

void draw(void)
{	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	//_mesh->drawWire();
	//_mesh->drawPoint();
	//_mesh->drawSolid(_smoothing);
	_mesh->drawSolidWithPing(_smoothing);
	glDisable(GL_LIGHTING);
}

void GL_Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5,0.5,0.5,0.5);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -_zoom); 
	glTranslatef(_translate_x, _translate_y, 0.0);
	glRotatef(_rotate_x, 1, 0, 0);
	glRotatef(_rotate_y, 0, 1, 0);

	draw();
	glutSwapBuffers();
}

void GL_Reshape(int w, int h)
{
	if (w == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void GL_Motion(int x, int y)
{
	int diff_x = x - last_x;
	int diff_y = y - last_y;

	last_x = x;
	last_y = y;

	if (_btnStates[2]) {
		_zoom -= (float) 0.05f * diff_x;
	}
	else if (_btnStates[0]) {
		_rotate_x += (float)0.5f * diff_y;
		_rotate_y += (float)0.5f * diff_x;
	}
	else if (_btnStates[1]) {
		_translate_x += (float)0.05f * diff_x;
		_translate_y -= (float)0.05f * diff_y;
	}
	glutPostRedisplay();
}

void GL_Mouse(int button, int state, int x, int y)
{
	last_x = x;
	last_y = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		_btnStates[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		_btnStates[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		_btnStates[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void GL_Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'Q':
	case 'q':
		exit(0);
	case 'S':
	case 's':
		_smoothing = !_smoothing;
		break;
	case 'K':
	case 'k':
		_mesh->Ctrl();
		break;
	case 'd':
	case 'D':
		_mesh->_Draw= !_mesh->_Draw;
	default:
		break;
	}
	glutPostRedisplay();
}


void main(int argc, char **argv)
{
	_mesh = new Mesh("E:/OpenGL/island.png","E:/OpenGL/texture_1_1.jpg");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Terrain Generator");
	glutDisplayFunc(GL_Display);
	glutReshapeFunc(GL_Reshape);
	glutMouseFunc(GL_Mouse);
	glutMotionFunc(GL_Motion);
	glutKeyboardFunc(GL_Keyboard);
	init();
	glutMainLoop();
}