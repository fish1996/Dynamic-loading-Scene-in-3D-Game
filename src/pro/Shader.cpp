#include "Shader.h"

#include <iostream>
#pragma comment(lib,"glew32.lib")  

GLuint Shader::vaoHandle = 0;
using namespace std;
//����λ������    
float positionData[] = {
	-0.5f,-0.5f,0.0f,1.0f,
	0.5f,-0.5f,0.0f,1.0f,
	0.5f,0.5f,0.0f,1.0f,
	-0.5f,0.5f,0.0f,1.0f
};
//������ɫ����    
float colorData[] = {
	1.0f, 0.0f, 0.0f,1.0f,
	0.0f, 1.0f, 0.0f,1.0f,
	0.0f, 0.0f, 1.0f,1.0f,
	1.0f,1.0f,0.0f,1.0f
};

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//ʹ��VAO��VBO����    
	glBindVertexArray(Shader::vaoHandle);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
	glutSwapBuffers();
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
	
	//���ض�����ɫ��
	addVertexShader();
	//����Ƭ����ɫ��
	addFragmentShader();
	linking();
	//�󶨲�����VAO��VBO  
	initVBO();
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Shader::linking()
{
	//3��������ɫ������    
	//������ɫ������    
	GLuint programHandle = glCreateProgram();
	if (!programHandle)
	{
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
	if (GL_FALSE == linkStatus)
	{
		cerr << "ERROR : link shader program failed" << endl;
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0)
		{
			char *log = (char *)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen,
				&written, log);
			cerr << "Program log : " << endl;
			cerr << log << endl;
		}
	}
	else//���ӳɹ�����OpenGL������ʹ����Ⱦ����    
	{
		glUseProgram(programHandle);
	}
}

void Shader::initVBO()
{
	//��VAO  
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	// Create and populate the buffer objects    
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	//��VBO�Թ�ʹ��    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	//�������ݵ�VBO    
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
		positionData, GL_STATIC_DRAW);

	//��VBO�Թ�ʹ��    
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	//�������ݵ�VBO    
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
		colorData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);//��������    
	glEnableVertexAttribArray(1);//������ɫ    

								 //����glVertexAttribPointer֮ǰ��Ҫ���а󶨲���    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	cout << "success" << endl;
}