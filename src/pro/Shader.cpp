#include "Shader.h"

#include <iostream>
#pragma comment(lib,"glew32.lib")  

GLuint Shader::vaoHandle[2] = { 0 };
int Shader::indicenum = 0;
using namespace std;
//顶点位置数组    
/*
float positionData[] = {
	-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.5f,0.5f,0.0f,
};
//顶点颜色数组    
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
	glEnableClientState(GL_VERTEX_ARRAY);//启用顶点坐标数组  
	glEnableClientState(GL_NORMAL_ARRAY);//启用法线向量数组  


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
	//创建着色器对象：顶点着色器    
	vShader = glCreateShader(GL_VERTEX_SHADER);
	if (vShader == 0) {
		cerr << "ERROR : Create vertex shader failed" << endl;
		exit(1);
	}

	//把着色器源代码和着色器对象相关联    
	const GLchar *vShaderCode = textFileRead(vertexPath);
	const GLchar *vCodeArray[1] = { vShaderCode };

	//将字符数组绑定到对应的着色器对象上  
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//编译着色器对象    
	glCompileShader(vShader);

	//检查编译是否成功    
	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		GLint logLen;
		//得到编译日志长度    
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(vShader, logLen, &written, log);
			cerr << "vertex shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间    
		}
	}
}

void Shader::addFragmentShader()
{
	//创建着色器对象：片断着色器    
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//错误检测    
	if (fShader == 0) {
		cerr << "ERROR : Create fragment shader failed" << endl;
		exit(1);
	}

	//把着色器源代码和着色器对象相关联    
	const GLchar *fShaderCode = textFileRead(fragmentPath);
	const GLchar *fCodeArray[1] = { fShaderCode };
	glShaderSource(fShader, 1, fCodeArray, NULL);

	//编译着色器对象    
	glCompileShader(fShader);

	//检查编译是否成功  
	GLint compileResult;
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult) {
		GLint logLen;
		//得到编译日志长度    
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(fShader, logLen, &written, log);
			cerr << "fragment shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间    
		}
	}
}

void Shader::printVersion()
{
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "显卡供应商   : " << vendor << endl;
	cout << "显卡型号     : " << renderer << endl;
	cout << "OpenGL版本   : " << version << endl;
	cout << "GLSL版本     : " << glslVersion << endl;
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

	//加载顶点着色器
	addVertexShader();
	//加载片段着色器
	addFragmentShader();
	linking();
	//绑定并加载VAO，VBO  
	initVBO();
}

void Shader::linking()
{   
	//创建着色器程序    
	GLuint programHandle = glCreateProgram();
	if (!programHandle) {
		cerr << "ERROR : create program failed" << endl;
		exit(1);
	}

	//将着色器程序链接到所创建的程序中    
	glAttachShader(programHandle, vShader);
	glAttachShader(programHandle, fShader);
	//将这些对象链接成一个可执行程序    
	glLinkProgram(programHandle);
	//查询链接的结果    
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
   
	// 绑定顶点
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle); 
	glBufferData(GL_ARRAY_BUFFER, vertexnum*sizeof(float),
		positionData, GL_STATIC_DRAW);
  
	// 绑定颜色
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle); 
	glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(float),
		colorData, GL_STATIC_DRAW);
	cout << vertexnum * sizeof(float) << " " << indicenum * sizeof(int) << endl;
  
	// 绑定索引
	glBindBuffer(GL_ARRAY_BUFFER, indiceBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, indicenum*sizeof(int),
		indiceData, GL_STATIC_DRAW);

	// 绑定法线
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, indicenum*sizeof(int),
		indiceData, GL_STATIC_DRAW);

	// 绑定VAO  
	glGenVertexArrays(1, vaoHandle);
	glBindVertexArray(vaoHandle[0]);

	glEnableVertexAttribArray(0);//顶点坐标    
	glEnableVertexAttribArray(1);//顶点颜色 
	glEnableVertexAttribArray(2);//顶点索引 
	glEnableVertexAttribArray(3);//顶点法线 

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
}