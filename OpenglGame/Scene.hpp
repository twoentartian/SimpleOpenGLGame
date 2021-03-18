#pragma once

#include <vector>
#include <unordered_map>
#include <exception>
#include <stdexcept>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm\ext.hpp>

#include "FPSCamera.hpp"
#include "SceneController.hpp"
#include "Shader.hpp"

struct SceneVertex
{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 texC;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	SceneVertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) : pos(p), norm(n), texC(t) {}
};

class sceneTarget
{
public:
	virtual void draw(Shader shader, GLuint diffuse, GLuint bump, GLuint spec, FPSCamera* cam) = 0;

	virtual void setBoundary() = 0;

	
	
};

class Cube
{
public:
	static GLuint CubeVAO, CubeVBO, CubeEBO;
	static std::vector<SceneVertex> CubeVertices;
	const static std::vector<unsigned int> CubeIndices;

	static void initCube(Shader shader)
	{
		constexpr GLfloat x = 0.5;
		constexpr GLfloat y = 0.5;
		constexpr GLfloat z = 0.5;

		//Front
		CubeVertices.push_back(SceneVertex(glm::vec3(x, y, z), glm::vec3(0, 0, 1), glm::vec2(1, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, -y, z), glm::vec3(0, 0, 1), glm::vec2(1, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, -y, z), glm::vec3(0, 0, 1), glm::vec2(0, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, y, z), glm::vec3(0, 0, 1), glm::vec2(0, 0)));

		//Right
		CubeVertices.push_back(SceneVertex(glm::vec3(x, y, z), glm::vec3(1, 0, 0), glm::vec2(0, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, y, -z), glm::vec3(1, 0, 0), glm::vec2(1, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, -y, -z), glm::vec3(1, 0, 0), glm::vec2(1, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, -y, z), glm::vec3(1, 0, 0), glm::vec2(0, 1)));

		//Top
		CubeVertices.push_back(SceneVertex(glm::vec3(x, y, z), glm::vec3(0, 1, 0), glm::vec2(0, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, y, z), glm::vec3(0, 1, 0), glm::vec2(0, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, y, -z), glm::vec3(0, 1, 0), glm::vec2(1, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, y, -z), glm::vec3(0, 1, 0), glm::vec2(1, 0)));

		//Back
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, -y, -z), glm::vec3(0, 0, -1), glm::vec2(1, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, -y, -z), glm::vec3(0, 0, -1), glm::vec2(0, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, y, -z), glm::vec3(0, 0, -1), glm::vec2(0, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, y, -z), glm::vec3(0, 0, -1), glm::vec2(1, 1)));

		//Left
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, -y, -z), glm::vec3(-1, 0, 0), glm::vec2(0, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, y, -z), glm::vec3(-1, 0, 0), glm::vec2(0, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, y, z), glm::vec3(-1, 0, 0), glm::vec2(1, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, -y, z), glm::vec3(-1, 0, 0), glm::vec2(1, 0)));

		//Bottom
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, -y, -z), glm::vec3(0, -1, 0), glm::vec2(0, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(-x, -y, z), glm::vec3(0, -1, 0), glm::vec2(1, 0)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, -y, z), glm::vec3(0, -1, 0), glm::vec2(1, 1)));
		CubeVertices.push_back(SceneVertex(glm::vec3(x, -y, -z), glm::vec3(0, -1, 0), glm::vec2(0, 1)));

		//Tangent space stuff
		for (int i = 0; i < CubeIndices.size(); i += 3)
		{
			SceneVertex& a = CubeVertices[CubeIndices[i]];
			SceneVertex& b = CubeVertices[CubeIndices[i + 1]];
			SceneVertex& c = CubeVertices[CubeIndices[i + 2]];

			glm::vec3 edge1 = b.pos - a.pos;
			glm::vec3 edge2 = c.pos - a.pos;
			glm::vec2 deltaUV1 = b.texC - a.texC;
			glm::vec2 deltaUV2 = c.texC - a.texC;

			GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 tangent, bitangent;

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent = glm::normalize(tangent);

			bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent = glm::normalize(bitangent);

			a.tangent = tangent;
			b.tangent = tangent;
			c.tangent = tangent;

			a.bitangent = bitangent;
			b.bitangent = bitangent;
			c.bitangent = bitangent;
		}

		glGenVertexArrays(1, &CubeVAO);
		glGenBuffers(1, &CubeVBO);
		glGenBuffers(1, &CubeEBO);

		glBindVertexArray(CubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SceneVertex) * CubeVertices.size(), &CubeVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * CubeIndices.size(), &CubeIndices[0], GL_STATIC_DRAW);

		//Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)0);

		//Normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)offsetof(SceneVertex, norm));

		//Tex2D
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)offsetof(SceneVertex, texC));

		//Tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)offsetof(SceneVertex, tangent));

		//Bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)offsetof(SceneVertex, bitangent));

		glBindVertexArray(0);
	}
	
	static void drawCube(Shader shader, GLuint diffuse, GLuint bump, GLuint spec)	//modern gl draw, init before use.
	{
		//Textures
		glActiveTexture(GL_TEXTURE0);	//0th texture unit
		glUniform1i(glGetUniformLocation(shader.Program, "diffuse_map"), 0);	//Bind "diffuse_map" in shader to 0th texture unit.
		glBindTexture(GL_TEXTURE_2D, diffuse);	//Bind texture to texture unit.

		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(shader.Program, "bump_map"), 1);
		glBindTexture(GL_TEXTURE_2D, bump);

		glActiveTexture(GL_TEXTURE2);
		glUniform1i(glGetUniformLocation(shader.Program, "spec_map"), 2);
		glBindTexture(GL_TEXTURE_2D, spec);

		//Draw
		glBindVertexArray(CubeVAO);
		glDrawElements(GL_TRIANGLES, CubeIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//Release Textures: in reverse order!
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_TEXTURE0);
	}
	
};

GLuint Cube::CubeVAO, Cube::CubeVBO, Cube::CubeEBO;
std::vector<SceneVertex> Cube::CubeVertices;
const std::vector<unsigned int> Cube::CubeIndices =
{
	0, 1, 2, 0, 2, 3,		//Front
	4, 5, 6, 4, 6, 7,		//Right
	8, 9, 10, 8, 10, 11,	//Back
	12, 13, 14, 12, 14, 15,	//Left
	16, 17, 18, 16, 18, 19,	//Top
	20, 21, 22, 20, 22, 23	//Bottom
};

class sceneBox : sceneTarget
{
public:
	glm::vec3 boxPosition;
	glm::vec3 boxScale;
	glm::vec3 boxRotateAxis;
	float boxRotateRadius;
	glm::mat4 boxLastRotateResult;
	glm::vec3 boxRotateAxis2;
	float boxRotateRadius2;
	glm::mat4 boxLastRotateResult2;
	glm::vec3 boxRotateAxis3;
	float boxRotateRadius3;
	glm::mat4 boxLastRotateResult3;
	std::string name;

	static std::unordered_map<std::string,std::shared_ptr<sceneBox>> AllSceneBox;

	sceneBox(const std::string& name_, const glm::vec3& pos, const glm::vec3& scale) : name(name_), boxPosition(pos), boxScale(scale), boxRotateAxis(glm::vec3(1,0,0)), boxRotateRadius(0), boxRotateRadius2(0), boxRotateRadius3(0)
	{
		boxLastRotateResult = glm::mat4(1.0);
		boxLastRotateResult2 = glm::mat4(1.0);
		boxLastRotateResult3 = glm::mat4(1.0);
	};
	
	static void Container_AllDraw(Shader shader, GLuint diffuse, GLuint bump, GLuint spec, FPSCamera* cam)
	{
		updateGl(shader, diffuse, bump, spec, cam);
		for (const auto& box : AllSceneBox)
		{
			box.second->_drawWithoutUpdateGl(shader, diffuse, bump, spec, cam);
		}
	}
	
	static void Container_Add(const std::string& name, const glm::vec3& pos, const glm::vec3& scale)
	{
		if (AllSceneBox.find(name) != AllSceneBox.end())
		{
			throw std::logic_error("Boxes with same name");
		}
		AllSceneBox.emplace(std::make_pair(name, std::make_shared<sceneBox>(name, pos, scale)));
	}

	static std::shared_ptr<sceneBox> Container_Get(const std::string& name)
	{
		const auto output = AllSceneBox.find(name);
		if (output == AllSceneBox.end())
		{
			throw std::logic_error("Name does not exist");
		}
		return output->second;
	}

	~sceneBox()
	{
		auto iter = AllSceneBox.find(name);
		AllSceneBox.erase(iter);
	}

	void draw(Shader shader, GLuint diffuse, GLuint bump, GLuint spec, FPSCamera* cam) override
	{
		updateGl(shader, diffuse, bump, spec, cam);
		_drawWithoutUpdateGl(shader, diffuse, bump, spec, cam);
	}

	void setBoundary() override
	{
		
	}

	glm::mat4 getMappingMatrix4()
	{
		glm::mat4 boxPos = glm::translate(glm::mat4(1.0), glm::vec3(boxPosition.x, boxPosition.y, boxPosition.z));
		boxLastRotateResult = glm::rotate(glm::mat4(1.0), glm::radians(boxRotateRadius), boxRotateAxis);
		boxLastRotateResult2 = glm::rotate(glm::mat4(1.0), glm::radians(boxRotateRadius2), boxRotateAxis2);
		boxLastRotateResult3 = glm::rotate(glm::mat4(1.0), glm::radians(boxRotateRadius3), boxRotateAxis3);
		glm::mat4 boxScl = glm::scale(glm::mat4(1.0), glm::vec3(boxScale.x, boxScale.y, boxScale.z));
		return boxPos * boxLastRotateResult * boxLastRotateResult2 * boxLastRotateResult3 * boxScl;
	}

	glm::mat3 getMappingMatrix3()
	{
		glm::mat4 output4 = getMappingMatrix4();
		glm::mat3 output;
		for (int x = 0; x < 3; ++x)
		{
			for (int y = 0; y < 3; ++y)
			{
				output[x][y] = output4[x][y];
			}
		}
		return output;
	}

private:
	
	static void updateGl(Shader shader, GLuint diffuse, GLuint bump, GLuint spec, FPSCamera* cam)
	{
		glUniform3fv(glGetUniformLocation(shader.Program, "lightPos1"), 1, LightPosition);
		glUniform3fv(glGetUniformLocation(shader.Program, "lightPos2"), 1, LightPosition2);
		glUniform3fv(glGetUniformLocation(shader.Program, "lightPos3"), 1, LightPosition3);
		glUniform3fv(glGetUniformLocation(shader.Program, "lightPos4"), 1, LightPosition4);
		glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, glm::value_ptr(cam->cameraPos));

		float P[16];
		glGetFloatv(GL_PROJECTION_MATRIX, P);
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, P);
	}
	
	void _drawWithoutUpdateGl(Shader shader, GLuint diffuse, GLuint bump, GLuint spec, FPSCamera* cam)
	{	

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr( getMappingMatrix4()));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(cam->viewMatrix));

		Cube::drawCube(shader, diffuse, bump, spec);
	}
};
std::unordered_map<std::string, std::shared_ptr<sceneBox>> sceneBox::AllSceneBox;
