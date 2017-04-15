#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define GLUT_DISABLE_ATEXIT_HACK
#include "textfile.h"
#include "glew.h"
#include "glut.h"

class Shader {
private:
	GLuint vShader, fShader;//����/Ƭ����ɫ������    
	
	char *vertexPath;
	char *fragmentPath;
	void addVertexShader();
	void addFragmentShader();
	void linking();
	void initVBO();
	friend void display();
public:
	static GLuint vaoHandle;// VAO����  
	void initialize();
	void printVersion();
	Shader::Shader(const char* verPath, const char* fragPath);
};