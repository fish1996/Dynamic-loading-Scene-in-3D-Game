#include "Shader.h"

#include <iostream>
#pragma comment(lib,"glew32.lib")  

GLuint Shader::vaoHandle[2] = { 0 };
int Shader::indicenum = 0;
using namespace std;
//����λ������    
/*
float positionData[] = {
	-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.5f,0.5f,0.0f,
};
//������ɫ����    
float colorData[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
};
*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(Shader::vaoHandle[0]);
	glEnableClientState(GL_VERTEX_ARRAY);//���ö�����������  
	glEnableClientState(GL_NORMAL_ARRAY);//���÷�����������  


	glDrawElements(GL_TRIANGLES, Shader::indicenum, GL_UNSIGNED_INT, 0);


	glutSwapBuffers();
	glutPostRedisplay();

}

Shader::Shader(const char* verPath, const char* fragPath)
{
	
	vertexPath = new char[strlen(verPath) + 1];
	strcpy(vertexPath, verPath);
	fragmentPath = new char[strlen(fragPath) + 1];
	strcpy(fragmentPath, fragPath);

}


void Shader::addVertexShader()
{
	//������ɫ�����󣺶�����ɫ��    
	vShader = glCreateShader(GL_VERTEX_SHADER);
	if (vShader == 0) {
		cerr << "ERROR : Create vertex shader failed" << endl;
		exit(1);
	}

	//����ɫ��Դ�������ɫ�����������    
	const GLchar *vShaderCode = textFileRead(vertexPath);
	const GLchar *vCodeArray[1] = { vShaderCode };

	//���ַ�����󶨵���Ӧ����ɫ��������  
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//������ɫ������    
	glCompileShader(vShader);

	//�������Ƿ�ɹ�    
	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		GLint logLen;
		//�õ�������־����    
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//�õ���־��Ϣ�����    
			glGetShaderInfoLog(vShader, logLen, &written, log);
			cerr << "vertex shader compile log : " << endl;
			cerr << log << endl;
			free(log);//�ͷſռ�    
		}
	}
}

void Shader::addFragmentShader()
{
	//������ɫ������Ƭ����ɫ��    
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//������    
	if (fShader == 0) {
		cerr << "ERROR : Create fragment shader failed" << endl;
		exit(1);
	}

	//����ɫ��Դ�������ɫ�����������    
	const GLchar *fShaderCode = textFileRead(fragmentPath);
	const GLchar *fCodeArray[1] = { fShaderCode };
	glShaderSource(fShader, 1, fCodeArray, NULL);

	//������ɫ������    
	glCompileShader(fShader);

	//�������Ƿ�ɹ�  
	GLint compileResult;
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult) {
		GLint logLen;
		//�õ�������־����    
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//�õ���־��Ϣ�����    
			glGetShaderInfoLog(fShader, logLen, &written, log);
			cerr << "fragment shader compile log : " << endl;
			cerr << log << endl;
			free(log);//�ͷſռ�    
		}
	}
}

void Shader::printVersion()
{
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "�Կ���Ӧ��   : " << vendor << endl;
	cout << "�Կ��ͺ�     : " << renderer << endl;
	cout << "OpenGL�汾   : " << version << endl;
	cout << "GLSL�汾     : " << glslVersion << endl;
}

void Shader::initialize()
{
	printVersion();
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << glewGetErrorString(err) << endl;
	}
	obj = new Object();
	obj->readObj("..\\..\\res\\obj\\test.obj", positionData,colorData,normalData,indiceData,indicenum,vertexnum);

	//���ض�����ɫ��
	addVertexShader();
	//����Ƭ����ɫ��
	addFragmentShader();
	linking();
	//�󶨲�����VAO��VBO  
	initVBO();
}

void Shader::linking()
{   
	//������ɫ������    
	GLuint programHandle = glCreateProgram();
	if (!programHandle) {
		cerr << "ERROR : create program failed" << endl;
		exit(1);
	}

	//����ɫ���������ӵ��������ĳ�����    
	glAttachShader(programHandle, vShader);
	glAttachShader(programHandle, fShader);
	//����Щ�������ӳ�һ����ִ�г���    
	glLinkProgram(programHandle);
	//��ѯ���ӵĽ��    
	GLint linkStatus;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
	if (GL_FALSE == linkStatus) {
		cerr << "ERROR : link shader program failed" << endl;
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0) {
			char *log = (char *)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen,
				&written, log);
			cerr << "Program log : " << endl;
			cerr << log << endl;
		}
	}
	else {
		glUseProgram(programHandle);
	}
}

void Shader::initVBO()
{
	GLuint vboHandles[4];
	glGenBuffers(4, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];
	GLuint indiceBufferHandle = vboHandles[2];
	GLuint normalBufferHandle = vboHandles[3];
   
	// �󶨶���
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle); 
	glBufferData(GL_ARRAY_BUFFER, vertexnum*sizeof(float),
		positionData, GL_STATIC_DRAW);
  
	// ����ɫ
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle); 
	glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(float),
		colorData, GL_STATIC_DRAW);
	cout << vertexnum * sizeof(float) << " " << indicenum * sizeof(int) << endl;
  
	// ������
	glBindBuffer(GL_ARRAY_BUFFER, indiceBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, indicenum*sizeof(int),
		indiceData, GL_STATIC_DRAW);

	// �󶨷���
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, indicenum*sizeof(int),
		indiceData, GL_STATIC_DRAW);

	// ��VAO  
	glGenVertexArrays(1, vaoHandle);
	glBindVertexArray(vaoHandle[0]);

	glEnableVertexAttribArray(0);//��������    
	glEnableVertexAttribArray(1);//������ɫ 
	glEnableVertexAttribArray(2);//�������� 
	glEnableVertexAttribArray(3);//���㷨�� 

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
}