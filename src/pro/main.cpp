#include "Shader.h"
#include <iostream>
/*
using namespace std;
int main()
{
	int n;
	cin >> n;
	for (int i = 1; i <= pow(2,n)-1; i++) {
		int x = i << 1;
		int ans = x & (~(x - 1));
		int y = log2(ans);
		cout << "move " << y << " from" << " to"<<endl;
	}
}*/

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