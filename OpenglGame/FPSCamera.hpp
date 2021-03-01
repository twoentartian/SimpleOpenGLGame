#include <iostream>
#include <cmath>
#include <gl/freeglut.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include "PhysicsEngine.hpp"

class TranslationAndRotate
{
public:
	template<typename T>
	static glm::mat<4, 4, T, glm::defaultp> rotate(glm::mat<4, 4, T, glm::defaultp> const& m, const T& angle, glm::vec<3, T, glm::defaultp> const& v)
	{
		const T& c = cos(angle);
		const T& s = sin(angle);

		glm::vec<3, T, glm::defaultp> axis(glm::normalize(v));
		glm::vec<3, T, glm::defaultp> temp((T(1) - c) * axis);

		glm::mat<4, 4, T, glm::defaultp> Rotate;
		Rotate[0][0] = c + temp[0] * axis[0];
		Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = c + temp[1] * axis[1];
		Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = c + temp[2] * axis[2];

		glm::mat<4, 4, T, glm::defaultp> Result;
		Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
		Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
		Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
		Result[3] = m[3];
		return Result;
	}

	template<typename T>
	static glm::mat<4, 4, T, glm::defaultp> translate(glm::mat<4, 4, T, glm::defaultp> const& m, glm::vec<3, T, glm::defaultp> const& v)
	{
		glm::mat<4, 4, T, glm::defaultp> Result(m);
		Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
		return Result;
	}
	
};

class FPSCamera
{
public:
	const float CameraMoveSensitivity = 2;
	const float CameraRotateSensitivity = 0.01;
	
	FPSCamera()
	{
		isWPressed = isSPressed = isAPressed = isDPressed = false;

		pfov = 45.0;
		pratio = 1.0;
		pnear = 1.0;
		pfar = 1000.0;

		roll = 0;
		pitch = 0;
		yaw = 0;

		cameraPos = glm::vec3(-80.0f, -20.f, 90.0f);
		targetPos = glm::vec3(-80.0f, -20.f, 85.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		projectionMatrix = glm::perspective(pfov, pratio, pnear, pfar);
		glMultMatrixf((float*)glm::value_ptr(projectionMatrix));

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		viewMatrix = glm::lookAt(cameraPos,targetPos,glm::vec3(0.f, 1.0f, 0.f));
		glMultMatrixf((float*)glm::value_ptr(viewMatrix));
	}

	~FPSCamera() = default;

	void resetWinSize(int w, int h)    //窗口大小发生变化时回调
	{
		winW = w;
		winH = h;
		pratio = (GLfloat)w / (GLfloat)h;

		cout << "winW " << winW << " winH " << winH << endl;

		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(pfov, pratio, pnear, pfar);

		glMatrixMode(GL_MODELVIEW);
		updateView();
	}

	void rotate(GLfloat const pitchRad, GLfloat const yawRad)
	{
		pitch += pitchRad;
		yaw += yawRad;

		updateView();
	}

	void keyPressed(const unsigned char key)
	{		
		switch (key)
		{
			//jump
		case ' ':
			if (!physicsEngine.isJumping)
			{
				physicsEngine.jumpAndUpdateVelocity();
			}
			physicsEngine.isJumping = true;
			break;

			//move
		case 'W':
		case 'w':
			isWPressed = true;
			break;

		case 'S':
		case 's':
			isSPressed = true;
			break;

		case 'A':
		case 'a':
			isAPressed = true;
			break;

		case 'D':
		case 'd':
			isDPressed = true;
			break;

			//camera rotate
		case 'J':
		case 'j':
			isLeftPressed = true;
			break;

		case 'L':
		case 'l':
			isRightPressed = true;
			break;
			
		case 'I':
		case 'i':
			isUpPressed = true;
			break;
			
		case 'K':
		case 'k':
			isDownPressed = true;
			break;
			
		default:
			break;
		}
	}

	void keyUp(const unsigned char key)
	{
		switch (key)
		{
		case ' ':

			break;

			//move
		case 'W':
		case 'w':
			isWPressed = false;
			break;

		case 'S':
		case 's':
			isSPressed = false;
			break;

		case 'A':
		case 'a':
			isAPressed = false;
			break;

		case 'D':
		case 'd':
			isDPressed = false;
			break;

			//camera rotate
		case 'J':
		case 'j':
			isLeftPressed = false;
			break;

		case 'L':
		case 'l':
			isRightPressed = false;
			break;

		case 'I':
		case 'i':
			isUpPressed = false;
			break;

		case 'K':
		case 'k':
			isDownPressed = false;
			break;
			
		default:
			break;
		}
	}

	void setSceneOuterBoundary(float x1, float z1, float x2, float z2)    //设置空间外部边缘
	{
		physicsEngine.setSceneOuterBoundary(x1, z1, x2, z2);
	}

	void setSceneInnerBoundary(float x1, float y1, float z1, float x2, float y2, float z2)    //设置空间内部边缘
	{
		physicsEngine.setSceneInnerBoundary(x1, y1, z1, x2, y2, z2);
	}

	void updateCameraMovement()    //每帧绘制的时候更新摄像机移动
	{
		updateCameraHoriMovement();
		updateCameraVertMovement();
		updateCameraRotate();
		updateView();
	}

	glm::vec3 getForward()
	{
		return  -glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
	}

	//根据面包的位置判断是否能吃掉面包
	bool detectPlayerEatingBread(glm::vec3 breadPos, float dist)
	{
		float dx = (breadPos - cameraPos).x;
		float dy = (breadPos - cameraPos).y;
		float dz = (breadPos - cameraPos).z;
		float len = dx * dx + dy * dy + dz * dz;
		//cout << "length " << len << endl;

		if (len < dist)    //小于一定距离的时候能吃掉
			return true;
		return false;
	}

	//判断玩家是否靠近某个面包
	bool detectPlayerCloseToBread(glm::vec3 breadPos, float dist)
	{
		float dx = (breadPos - cameraPos).x;
		float dy = (breadPos - cameraPos).y;
		float dz = (breadPos - cameraPos).z;
		float len = dx * dx + dy * dy + dz * dz;
		//cout << "length " << len << endl;

		if (len < dist)    //小于一定距离的时候算作靠近
			return true;
		return false;
	}

	glm::vec3 cameraPos;
	glm::vec3 targetPos;

	//Current view matrix
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

private:
	int winH = 0;
	int winW = 0;

	bool isWPressed, isSPressed, isAPressed, isDPressed;
	bool isLeftPressed, isRightPressed, isUpPressed, isDownPressed;

	//Lens parameters for the camera
	GLfloat pfov, pratio, pnear, pfar;

	//Camera roll, pitch, yaw info.
	GLfloat roll, pitch, yaw;

	PhysicsEngine physicsEngine;
	
	void updateCameraHoriMovement()
	{
		float dx = 0, dz = 0;

		if (isWPressed)
			dz += CameraMoveSensitivity;
		if (isSPressed)
			dz -= CameraMoveSensitivity;
		if (isAPressed)
			dx -= CameraMoveSensitivity;
		if (isDPressed)
			dx += CameraMoveSensitivity;

		if (dz != 0 || dx != 0)
		{
			glm::vec3 forward = glm::vec3(viewMatrix[0][2], 0.f, viewMatrix[2][2]);
			glm::vec3 strafe = glm::vec3(viewMatrix[0][0], 0.f, viewMatrix[2][0]);

			cameraPos += (-dz * forward + dx * strafe) * PlayerMoveSpeed;
			targetPos = cameraPos + (-dz * forward + dx * strafe) * 1.5f;

			//perform CollisionTest to avoid camera entering some strange places.
			physicsEngine.outCollisionTest(cameraPos, targetPos);
			physicsEngine.inCollisionTest(cameraPos, targetPos);
		}
	}

	void updateCameraRotate()
	{
		float pitch = 0, yaw = 0;
		if (isLeftPressed)
			yaw += CameraRotateSensitivity;
		if (isRightPressed)
			yaw -= CameraRotateSensitivity;
		if (isUpPressed)
			pitch += CameraRotateSensitivity;
		if (isDownPressed)
			pitch -= CameraRotateSensitivity;
		rotate(pitch, yaw);
	}

	void updateCameraVertMovement()
	{
		physicsEngine.updateCameraVertMovement(cameraPos, targetPos);
	}

	void updateView()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glm::mat4 matroll = TranslationAndRotate::rotate(glm::mat4(1.0), -roll, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 matpitch = TranslationAndRotate::rotate(glm::mat4(1.0), -pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 matyaw = TranslationAndRotate::rotate(glm::mat4(1.0), -yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 mattranslate = TranslationAndRotate::translate(glm::mat4(1.0f), -cameraPos);

		viewMatrix = matroll * matpitch * matyaw * mattranslate;

		glMultMatrixf((float*)glm::value_ptr(viewMatrix));
	}
};