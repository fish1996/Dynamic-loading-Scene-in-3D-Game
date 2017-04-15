#include "Shader.h"
#include <iostream>

int main(int argc, char** argv)
{
	Shader* s = new Shader("../../res/VertexShader.vert", "../../res/FragmentShader.frag");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hello GLSL");
	glutDisplayFunc(display);
	s->initialize();
	glutMainLoop();
}