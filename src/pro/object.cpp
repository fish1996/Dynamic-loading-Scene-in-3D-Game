#define _CRT_SECURE_NO_WARNINGS

#include "object.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <string>
using namespace std;

static GLuint load_texture(const char *file_name);
static void ReadMtl(string &cd, string mtlfile, map<string, Material> &mat);
int power_of_two(int n)
{
	if (n <= 0) return 0;
	return (n&(n - 1)) == 0;
}

static GLuint load_texture(const char *file_name)
{
	int width, height;
	int total_bytes;
	GLuint last_texture_ID, texture_ID = 0;
	GLubyte *pixels = NULL;
	FILE *pFile;

	if ((pFile = fopen(file_name, "rb")) == NULL)
	{
		cout << "Read texture error" << endl;
		return 0;
	}

	fseek(pFile, 18, SEEK_SET);
	fread(&width, sizeof(width), 1, pFile);
	fread(&height, sizeof(height), 1, pFile);
	fseek(pFile, 54, SEEK_SET);

	total_bytes = (width * 3 + (4 - width * 3 % 4) % 4)*height;

	if ((pixels = (GLubyte *)malloc(total_bytes)) == NULL)
	{
		fclose(pFile);
		return 0;
	}

	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width) || !power_of_two(height) || width>max || height>max)
		{
			const GLint new_width = 1024;
			const GLint new_height = 1024;
			GLint new_total_bytes;
			GLubyte *new_pixels = NULL;

			new_total_bytes = (new_width * 3 + (4 - new_width * 3 % 4) % 4)*new_height;
			new_pixels = (GLubyte *)malloc(new_total_bytes);
			if (new_pixels == NULL)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			gluScaleImage(GL_RGB, width, height, GL_UNSIGNED_BYTE, pixels, new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);

	free(pixels);
	fclose(pFile);
	cout << texture_ID;
	return texture_ID;
}

void Object::readObj(string file,float* &vertexData,float* &colorData, float* &normalData,int* &indiceData,int &indicenum,int & vertexnum)
{
	ifstream in;
	vector< pair<float, float> > texcoords;
	vector<VERTEX> normals;
	vector<int> faces;
	int row = 0, col = 0;
	int i = 0;
	string line, word, goname, mtlname;
	string cd;
	char Buffer[MAX_PATH];

	if (file.find(":") != string::npos) {
		cd = string(file.begin(), file.begin() + file.rfind("\\"));
	}
	else if (startswith(file, string(".\\"))) {
		GetCurrentDirectoryA(MAX_PATH, Buffer);
		cd = Buffer + string(file.begin() + 1, file.begin() + file.rfind("\\"));
	}
	else if (startswith(file, string("..\\"))) {
		GetCurrentDirectoryA(MAX_PATH, Buffer);
		cd = Buffer;
		cd = string(cd.begin(), cd.begin() + cd.rfind("\\"));
		cd = cd + string(file.begin() + 2, file.begin() + file.rfind("\\"));
		cout << cd << endl;
	}
	else {
		GetCurrentDirectoryA(MAX_PATH, Buffer);
		if (file.rfind("\\") != string::npos) {
			cd = Buffer + string("\\") + string(file.begin(), file.begin() + file.rfind("\\"));
		}
		else {
			cd = Buffer;
		}
	}

	in.open(file.c_str());
	if (!in) {
		cout << "Read obj error !" << endl;
		exit(0);
	}
	while (getline(in, line)) {
		if (line.size() == 0 || line[0] == '#') continue;
		istringstream is(line);
		is >> word;
		if (word == "v") { //顶点
			VERTEX p;
			is >> p.x >> p.y >> p.z;
			vertexs.push_back(p);
		}
		else if (word == "vt") {
			pair<float, float> p;
			is >> p.first >> p.second;
			
		}
		else if (word == "vn") { //顶点法线
			VERTEX p;
			is >> p.x >> p.y >> p.z;
			normals.push_back(p);
		}
		else if (word == "o" || word == "g") {
			if (!goname.empty() && !faces.empty()) {

			}
			is >> goname;
		}
		else if (word == "f") {
			int r = 0, c = 0;
			while (is >> word) {
				c = count(word.begin(), word.end(), '/');
				if (c == 0) {
					faces.push_back(atoi(word.c_str()));
				}
				else if (c == 1) {
					faces.push_back(atoi(string(word.begin(), word.begin() + word.find("/")).c_str()));
					faces.push_back(atoi(string(word.begin() + word.find("/") + 1, word.end()).c_str()));
				}
				else if (c == 2) {
					int a = word.find("/");
					int b = word.find("/", a + 1);
					faces.push_back(atoi(string(word.begin(), word.begin() + a).c_str()));
					faces.push_back(atoi(string(word.begin() + a + 1, word.begin() + b).c_str()));
					faces.push_back(atoi(string(word.begin() + b + 1, word.end()).c_str()));
				}
				++r;
			}
			row = r;
			col = c + 1;
		}
		else if (word == "mtllib") {
			is >> word;
		//	ReadMtl(cd, word, matname);
		}
		else if (word == "usemtl") {
			is >> mtlname;
		}
	}
	if (!goname.empty() && !faces.empty()) {
	//	Object obj(vertexs.begin(), vertexs.end(), texcoords.begin(), texcoords.end(), normals.begin(), normals.end(), faces.begin(), faces.end(), row, col, mtlname);

	//	m.insert(make_pair(goname, obj));
	//	n.insert(goname);
//		faces.clear();
	}
	int count = 0;
	vertexData = new float[vertexs.size() * 3];
	colorData = new float[vertexs.size() * 3];
	normalData = new float[vertexs.size() * 3];
	for (int i = 0; i < vertexs.size(); i++) {
		colorData[count] = 1.0f;
		normalData[count] = normals[i].x;
		vertexData[count++] = vertexs[i].x;
		colorData[count] = 0.3f;
		normalData[count] = normals[i].y;
		vertexData[count++] = vertexs[i].y;
		colorData[count] = 0.0f;
		normalData[count] = normals[i].z;
		vertexData[count++] = vertexs[i].z;
		
	}
	count = 0;
	indiceData = new int[faces.size()];
	for (int i = 0; i < faces.size(); i++) {
		indiceData[count++] = faces[i];
	}

	vertexnum = vertexs.size()*3;
	indicenum = faces.size();
	vertexs.clear();
	in.close();
}

void setMaterial(Material &mat)
{
	std::cout << mat.ambient << std::endl;;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat.emission);
	glMaterialf(GL_FRONT, GL_SHININESS, 100);
}


static void ReadMtl(string &cd, string mtlfile, map<string, Material> &mat)
{
	ifstream in;
	string line, word, ptname, ntname, fname;
	unsigned map;
	bool hasmap = false;
	float ambient[3] = { 0.0,0.0,0.0 };
	float diffuse[3] = { 0.0,0.0,0.0 };
	float specular[3] = { 0.0,0.0,0.0 };
	float emission[3] = { 0.0,0.0,0.0 };

	in.open((cd + "\\" + mtlfile).c_str());
	if (!in) {
		cout << "Read mtl error !" << endl;
		return;
	}
	while (getline(in, line)) {
		if (line.size() == 0 || line[0] == '#') continue;
		istringstream is(line);
		is >> word;
		if (word == "newmtl") {
			is >> ntname;
			if (!ptname.empty()) {
				if (hasmap) {
					printf("hasMap\n");
					mat.insert(make_pair(ptname, Material(ambient, diffuse, specular, emission, map)));
				}
				else {
					mat.insert(make_pair(ptname, Material(ambient, diffuse, specular, emission, 0)));
				}
			}
			ptname = ntname;
			hasmap = false;
		}
		else if (word == "Ka") {
			is >> ambient[0] >> ambient[1] >> ambient[2];
		}
		else if (word == "Kd") {
			is >> diffuse[0] >> diffuse[1] >> diffuse[2];
		}
		else if (word == "Ks") {
			is >> specular[0] >> specular[1] >> specular[2];
		}
	
		else if (word == "Ke") {
			is >> emission[0] >> emission[1] >> emission[2];
		}
		else if (word == "map_Kd" || word == "map_Ka") {
			is >> fname;
			map = load_texture((cd + "\\" + fname).c_str());
			hasmap = true;
		}
	}
	if (!ntname.empty()) {
		if (hasmap) {
			mat.insert(make_pair(ptname, Material(ambient, diffuse, specular, emission, map)));
		}
		else {
			mat.insert(make_pair(ptname, Material(ambient, diffuse, specular, emission, 0)));
		}
	}
	in.close();
}
