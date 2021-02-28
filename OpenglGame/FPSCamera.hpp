#include <iostream>
#include <cmath>
#include <gl/freeglut.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include "PhysicsEngine.hpp"
using namespace std;

class FPSCamera
{
public:

	FPSCamera()
	{
		physicsEngine = new PhysicsEngine;

		isWPressing = isSPressing = isAPressing = isDPressing = false;

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
		viewMatrix = glm::lookAt(
			cameraPos,
			targetPos,
			glm::vec3(0.f, 1.0f, 0.f)
		);
		glMultMatrixf((float*)glm::value_ptr(viewMatrix));
	}

	~FPSCamera()
	{
		delete physicsEngine;
	}

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

	void keyPressed(const unsigned char key)       //按键按下
	{
		switch (key) {
		case ' ':
			cout << "space press!" << endl;
			if (!physicsEngine->isJumping) {
				physicsEngine->jumpAndUpdateVelocity();

			}
			physicsEngine->isJumping = true;
			break;

		case 'W':
		case 'w':
			isWPressing = true;
			break;

		case 'S':
		case 's':
			isSPressing = true;
			break;

		case 'A':
		case 'a':
			isAPressing = true;
			break;

		case 'D':
		case 'd':
			isDPressing = true;
			break;
		default:
			break;
		}
	}

	void keyUp(const unsigned char key)            //按键抬起
	{
		switch (key) {
		case ' ':

			break;

		case 'W':
		case 'w':
			isWPressing = false;
			break;

		case 'S':
		case 's':
			isSPressing = false;
			break;

		case 'A':
		case 'a':
			isAPressing = false;
			break;

		case 'D':
		case 'd':
			isDPressing = false;
			break;
		default:
			break;
		}
	}

	void setSceneOuterBoundary(float x1, float z1, float x2, float z2)    //设置空间外部边缘
	{
		physicsEngine->setSceneOuterBoundary(x1, z1, x2, z2);
	}

	void setSceneInnerBoundary(float x1, float y1, float z1, float x2, float y2, float z2)    //设置空间内部边缘
	{
		physicsEngine->setSceneInnerBoundary(x1, y1, z1, x2, y2, z2);
	}

	void updateCameraMovement()    //每帧绘制的时候更新摄像机移动
	{
		updateCameraHoriMovement();
		updateCameraVertMovement();
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
	void updateCameraHoriMovement()        //每帧绘制的时候更新摄像机水平方向移动
	{
		float dx = 0;
		float dz = 0;

		if (isWPressing)
			dz += 2;
		if (isSPressing)
			dz -= 2;
		if (isAPressing)
			dx -= 2;
		if (isDPressing)
			dx += 2;

		if (dz != 0 || dx != 0) {
			//行走不改变y轴坐标
			glm::vec3 forward = glm::vec3(viewMatrix[0][2], 0.f, viewMatrix[2][2]);
			glm::vec3 strafe = glm::vec3(viewMatrix[0][0], 0.f, viewMatrix[2][0]);

			cameraPos += (-dz * forward + dx * strafe) * MoveSpeed;
			targetPos = cameraPos + (-dz * forward + dx * strafe) * 1.5f;

			//每次做完坐标变换后，先进行碰撞检测来调整坐标
			physicsEngine->outCollisionTest(cameraPos, targetPos);
			physicsEngine->inCollisionTest(cameraPos, targetPos);
		}
	}

	void updateCameraVertMovement()        //每帧绘制的时候更新摄像机垂直方向移动
	{
		physicsEngine->updateCameraVertMovement(cameraPos, targetPos);
	}

	void updateView()                      //更新视角
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glm::mat4 matroll = glm::rotate(glm::mat4(1.0), -roll, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 matpitch = glm::rotate(glm::mat4(1.0), -pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 matyaw = glm::rotate(glm::mat4(1.0), -yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 mattranslate = glm::translate(glm::mat4(1.0f), -cameraPos);

		viewMatrix = matroll * matpitch * matyaw * mattranslate;

		glMultMatrixf((float*)glm::value_ptr(viewMatrix));
	}

	int winH = 0;
	int winW = 0;

	bool isWPressing, isSPressing, isAPressing, isDPressing;


	//Lens parameters for the camera
	GLfloat pfov, pratio, pnear, pfar;

	//Camera roll, pitch, yaw info.
	GLfloat roll, pitch, yaw;

	PhysicsEngine* physicsEngine;

};