#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>  
#include <glm/gtx/string_cast.hpp>
#include <chrono>
#include <thread>

#include "SceneController.hpp"

constexpr float MouseSensitivity = 0.003; // mouse move sensitivity

enum GameStatus { MenuScene, GameScene };

FPSCamera* cam;

static int mouseLastPosX = 0;
static int mouseLastPosY = 0;

GLuint texture[10];
Shader boxShader;

void drawScene()
{
	boxShader.Use();
	glStencilMask(0x00);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	//drawBoxColliders(boxShader, texture[7], texture[8], texture[9], cam);	//Modern GL
	glDisable(GL_CULL_FACE);
	glUseProgram(NULL);
	
	//天空盒
	glStencilMask(0x00);
	Skybox::drawSkybox(texture);

	//地板  
	glPushMatrix();
	glStencilMask(0x00);
	glTranslatef(0.0f, -roomSizeY / 2.0f, 0.0f);
	glRotatef(90, 1, 0, 0);
	glScalef(roomSizeX, roomSizeZ, 1);
	Draw::drawRect(texture[0]);
	glPopMatrix();

	//箱子
	//drawBreadModels();
	//playBreadEatenEffect(cam);

	//glColor3f(1, 1, 1);

	//文字
	//drawGameSceneUIText(cam, 50, 550);

	cam->updateCameraMovement();
	//detectBreadBeingEaten(cam);
}

void reshape(int width, int height)
{
	cam->resetWinSize(width, height);    
}

void idle()
{
	//Redraw only runs for every 10ms
	static auto last_time = std::chrono::system_clock::now();
	std::this_thread::sleep_until(last_time + std::chrono::milliseconds(10));
	last_time = std::chrono::system_clock::now();
	glutPostRedisplay();
}

void initTexture()
{
	glEnable(GL_DEPTH_TEST);
	glGenTextures(10, texture);
	loadTex(0, "Textures/14.bmp", texture);

	//skybox
	loadTex(1, "Textures/Skybox1/up.png", texture);
	loadTex(2, "Textures/Skybox1/down.png", texture);
	loadTex(3, "Textures/Skybox1/left.png", texture);
	loadTex(4, "Textures/Skybox1/right.png", texture);
	loadTex(5, "Textures/Skybox1/front.png", texture);
	loadTex(6, "Textures/Skybox1/back.png", texture);

	boxShader.Use();
	loadTex(7, "Textures/19d.bmp", texture);	//Box Diffuse
	loadTex(8, "Textures/20b.bmp", texture);	//Box Bump
	loadTex(9,"Textures/21s.bmp", texture);		//Box Specular
	glUseProgram(NULL);
}

#pragma region UserInterface

void normalKeyPress(unsigned char key, int x, int y)
{
	cam->keyPressed(key);
}

void normalKeyUp(unsigned char key, int x, int y)
{
	cam->keyUp(key);
}

void mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseLastPosY = y;
		mouseLastPosX = x;
	}
}

void mouseMove(int x, int y)
{
	float pitch = (float)(mouseLastPosY - y) * MouseSensitivity;
	float yaw = (float)(mouseLastPosX - x) * MouseSensitivity;
	mouseLastPosY = y;
	mouseLastPosX = x;
	cam->rotate(pitch, yaw);
}

#pragma endregion

void redraw()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawScene();
	glutSwapBuffers();
}

void initializeGL()
{
	glewInit();
	glewExperimental = GL_TRUE;

	cam = new FPSCamera();
	//添加碰撞边缘
	cam->setSceneOuterBoundary(-roomSizeX / 2.0, -roomSizeZ / 2.0, roomSizeX / 2.0, roomSizeZ / 2.0);

	cam->setSceneInnerBoundary(-roomSizeX / 2.0,  -roomSizeY / 2.0f - 1.f, -roomSizeZ / 2.0,	roomSizeX / 2.0, -roomSizeY / 2.0f, roomSizeZ / 2.0);    //地板collider

	//initBoxCollidersProperty();
	//setBoxColliderBoundary(cam);

	//initBreadModels();

	setupLights();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	boxShader.load("shader.vert", "shader.frag");
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);    
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
	glutInitWindowPosition(250, 100);
	glutInitWindowSize(600, 600);
	int windowHandle = glutCreateWindow("IN4152 3D Computer Graphics and Animation lab 1");

	initializeGL();

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(normalKeyPress);
	glutKeyboardUpFunc(normalKeyUp);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutIdleFunc(idle);

	//boxShader.Use();
	//initCube(boxShader);
	//glUseProgram(NULL);

	initTexture();
	glutMainLoop();

	delete cam;
	//deleteBreadModels();

	return 0;
}