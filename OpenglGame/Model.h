#pragma once

#ifndef MODEL_H  
#define MODEL_H 

#include <fstream>
#include <iostream>
#include <string.h>
#include <map>

#include <gl/freeglut.h>
#include <IL/il.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


using namespace std;

class Model {
public:
	Model();
	~Model();

	bool importModel(const string& pFile);        //����ģ��

	void renderTheModel(float scale, bool isAmbient);             //��Ⱦģ�����

private:
	//����texture
	bool loadTextures(const aiScene* scene, const string modelPath);

	//�ݹ���Ⱦģ��
	void recursiveRender(const struct aiScene *sc, const struct aiNode* nd, float scale, bool isAmbient);

	//��ģ�����texture
	void applyMaterial(const aiMaterial *mtl, bool isAmbient);

private:
	const aiScene* scene = NULL;
	GLuint scene_list = 0;
	aiVector3D scene_min, scene_max, scene_center;

	map<string, GLuint*> textureIdMap;    // map image filenames to textureIds
	GLuint* textureIds;					  // pointer to texture Array

};

#endif

