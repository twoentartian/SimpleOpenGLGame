#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
using namespace std;

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) < (y) ? (y) : (x))


constexpr float BoundaryGap = 1.0f;          //碰撞间距
constexpr float GravityAcc = -9.8f;

constexpr float PlayerHeight = 7.5f;         //玩家视点到脚的高度
constexpr float PlayerMoveSpeed = 0.15f;     //玩家移动速度

constexpr float JumpInitialSpeed = 24.0f;    //起跳初速度
constexpr float JumpFactor = 0.04f;          //跳起速度系数
constexpr float GravityFactor = 0.04f;       //下落速度系数

struct dot
{
	float x;
	float y;
	dot(float _x, float _y) :x(_x), y(_y) { }
};

class SegmentUtil
{
public:
	//检测线段相交快速算法
	static inline bool SegmentIntersect(dot p1, dot p2, dot p3, dot p4)
	{
		int d1, d2, d3, d4;
		d1 = Direction(p3, p4, p1);
		d2 = Direction(p3, p4, p2);
		d3 = Direction(p1, p2, p3);
		d4 = Direction(p1, p2, p4);
		if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2>0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4>0)))
			return true;
		else if (d1 == 0 && OnSegment(p3, p4, p1))
			return true;
		else if (d2 == 0 && OnSegment(p3, p4, p2))
			return true;
		else if (d3 == 0 && OnSegment(p1, p2, p3))
			return true;
		else if (d4 == 0 && OnSegment(p1, p2, p4))
			return true;
		else
			return false;
	}

	//判断在xz平面，相机位置是否位于碰撞体内部
	static inline bool insideTheCollider(glm::vec3 _cameraPos, glm::vec3 _innerMin, glm::vec3 _innerMax)
	{
		float camX = _cameraPos.x;
		float camZ = _cameraPos.z;
		float minX = _innerMin.x;
		float minZ = _innerMin.z;
		float maxX = _innerMax.x;
		float maxZ = _innerMax.z;

		if (minX <= camX && camX <= maxX && minZ <= camZ && camZ <= maxZ)
			return true;
		else
			return false;
	}

private:
	static inline double Direction(dot pi, dot pj, dot pk)
	{
		return (pk.x - pi.x) * (pj.y - pi.y) - (pj.x - pi.x) * (pk.y - pi.y);
	}

	static inline bool OnSegment(dot pi, dot pj, dot pk)
	{
		if ((min(pi.x, pj.x) <= pk.x) && (pk.x <= max(pi.x, pj.x)) && (min(pi.y, pj.y) <= pk.y) && (pk.y <= max(pi.y, pj.y)))
			return true;
		else
			return false;
	}
};

class PhysicsEngine
{
public:
	bool isJumping;
	
	PhysicsEngine()
	{
		velocity = glm::vec3(0.f, 0.f, 0.f);
		gravity = glm::vec3(0.f, GravityAcc, 0.f);
		accelerUp = glm::vec3(0.f, 0.f, 0.f);

		isJumping = false;
	}
	
	~PhysicsEngine() = default;

	//设置空间外部边缘
	void setSceneOuterBoundary(float x1, float z1, float x2, float z2)
	{
		outerBoundary = glm::vec4(x1, z1, x2, z2);
	}
	
	//外部碰撞检测
	void outCollisionTest(glm::vec3 & cameraPos, glm::vec3 & targetPos)
	{
		outCollisionTestXZ(outerBoundary[0], outerBoundary[1], outerBoundary[2], outerBoundary[3], cameraPos, targetPos);
	}
	
	//设置空间内部边缘
	void setSceneInnerBoundary(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		glm::vec3 key(x1 - BoundaryGap, y1 - BoundaryGap, z1 - BoundaryGap);
		glm::vec3 value(x2 + BoundaryGap, y2 + BoundaryGap, z2 + BoundaryGap);

		innerBoundaryMin.push_back(key);
		innerBoundaryMax.push_back(value);
	}
	
	//内部碰撞检测
	void inCollisionTest(glm::vec3 & cameraPos, glm::vec3 & targetPos)
	{
		//后面可以在这里添加：预处理，排除当前肯定不会产生碰撞的物体
		for (int i = 0; i < innerBoundaryMin.size(); i++)
		{
			inCollisionTestWithHeight(innerBoundaryMin[i][0], innerBoundaryMin[i][1], innerBoundaryMin[i][2],innerBoundaryMax[i][0], innerBoundaryMax[i][1], innerBoundaryMax[i][2], cameraPos, targetPos);
		}
	}

	void jumpAndUpdateVelocity()    //按下space跳跃时调用
	{
		velocity += glm::vec3(0.0f, JumpInitialSpeed, 0.0f);
		accelerUp.y = 0.0f;
	}
	
	//每帧绘制的时候更新摄像机垂直方向移动
	void updateCameraVertMovement(glm::vec3 & cameraPos, glm::vec3 & targetPos)
	{
		glm::vec3 acceleration = gravity + accelerUp;
		velocity += acceleration * GravityFactor;
		cameraPos += velocity * JumpFactor;
		targetPos += velocity * JumpFactor;


		//检测所有碰撞体
		for (int i = 0; i < innerBoundaryMin.size(); i++)
		{
			//如果在XZ平面进入碰撞体所在区域
			if (SegmentUtil::insideTheCollider(cameraPos, innerBoundaryMin[i], innerBoundaryMax[i]))
			{
				if (cameraPos.y - PlayerHeight <= innerBoundaryMax[i][1] && cameraPos.y >= innerBoundaryMax[i][1])
				{
					isJumping = false;
					accelerUp.y = -GravityAcc;
					velocity.y = 0.f;
					cameraPos.y = innerBoundaryMax[i][1] + PlayerHeight;
					break;
				}

				if (cameraPos.y >= innerBoundaryMin[i][1] && cameraPos.y - PlayerHeight <= innerBoundaryMin[i][1])
				{
					velocity.y = 0.f;
					cameraPos.y = innerBoundaryMin[i][1];
					break;
				}
			}
			else
			{
				accelerUp.y = 0.f;
			}
		}
	}

private:
	//空间内部边缘碰撞检测(考虑高度)
	void inCollisionTestWithHeight(float x1, float y1, float z1, float x2, float y2, float z2, glm::vec3 & cameraPos, glm::vec3 & targetPos)
	{
		//当身体处于碰撞体垂直区域范围内，才进行XZ平面的碰撞检测
		if (!(cameraPos[1] <= y1 || cameraPos[1] - PlayerHeight >= y2))
		{
			inCollisionTestXZ(x1, z1, x2, z2, cameraPos, targetPos);
		}
	}
	
	//空间内部边缘碰撞检测（不考虑高度，即XZ平面）
	void inCollisionTestXZ(float x1, float z1, float x2, float z2, glm::vec3 & cameraPos, glm::vec3 & targetPos)
	{
		const float d = 2.0f;
		float tarX = targetPos[0], camX = cameraPos[0], tarZ = targetPos[2], camZ = cameraPos[2];
		float len = sqrt((camX - tarX) * (camX - tarX) + (camZ - tarZ) * (camZ - tarZ));

		dot d1(cameraPos[0], cameraPos[2]), d2(targetPos[0], targetPos[2]);
		dot d3(x1, z1), d4(x1, z2), d5(x2, z1), d6(x2, z2);

		if (SegmentUtil::SegmentIntersect(d1, d2, d4, d6))
		{
			if (targetPos[2] < cameraPos[2])
			{
				targetPos[2] = z2;
				cameraPos[2] += (targetPos[2] - tarZ);
			}
			else if (targetPos[2] > cameraPos[2])
			{
				cameraPos[2] = z2;
				targetPos[2] += (cameraPos[2] - camZ);
			}
		}
		else if (SegmentUtil::SegmentIntersect(d1, d2, d5, d6))
		{
			if (targetPos[0] < cameraPos[0])
			{
				targetPos[0] = x2;
				cameraPos[0] += (targetPos[0] - tarX);
			}
			else if (targetPos[0] > cameraPos[0])
			{
				cameraPos[0] = x2;
				targetPos[0] += (cameraPos[0] - camX);
			}
		}
		else if (SegmentUtil::SegmentIntersect(d1, d2, d3, d5))
		{
			if (targetPos[2] > cameraPos[2])
			{
				targetPos[2] = z1;
				cameraPos[2] += (targetPos[2] - tarZ);
			}
			else if (targetPos[2] < cameraPos[2])
			{
				cameraPos[2] = z1;
				targetPos[2] += (cameraPos[2] - camZ);
			}
		}
		else if (SegmentUtil::SegmentIntersect(d1, d2, d3, d4))
		{
			if (targetPos[0] > cameraPos[0])
			{
				targetPos[0] = x1;
				cameraPos[0] += (targetPos[0] - tarX);
			}
			else if (targetPos[0] < cameraPos[0])
			{
				cameraPos[0] = x1;
				targetPos[0] += (cameraPos[0] - camX);
			}
		}
	}
	
	//空间外部边缘碰撞检测
	void outCollisionTestXZ(float x1, float z1, float x2, float z2, glm::vec3 & cameraPos, glm::vec3 & targetPos)
	{
		//先设置包围盒：比空间外部边缘小一点
		if (x1 < 0)
			x1 += 2;
		else x1 -= 2;

		if (x2 < 0)
			x2 += 2;
		else x2 -= 2;

		if (z1 < 0)
			z1 += 2;
		else z1 -= 2;

		if (z2 < 0)
			z2 += 2;
		else z2 -= 2;

		//如果目标位置出了包围盒，先放回来
		if (targetPos[0] < x1)
		{
			targetPos[0] = x1;
		}
		if (targetPos[0] > x2)
		{
			targetPos[0] = x2;
		}
		if (targetPos[2] < z1)
		{
			targetPos[2] = z1;
		}
		if (targetPos[2] > z2)
		{
			targetPos[2] = z2;
		}

		float distance = sqrt((cameraPos[0] - targetPos[0]) * (cameraPos[0] - targetPos[0]) + (cameraPos[2] - targetPos[2]) * (cameraPos[2] - targetPos[2]));

		//若视点与目标距离太小，则固定目标位置，视点沿正对目标的逆方向移动
		if (distance <= 2.0f)
		{
			cameraPos[0] = 2.0f * (cameraPos[0] - targetPos[0]) / distance + targetPos[0];
			cameraPos[2] = 2.0f * (cameraPos[2] - targetPos[2]) / distance + targetPos[2];
		}
		bool flag = false;

		//再检测视点是否出了包围盒，若是则放回
		if (cameraPos[0] < x1)
		{
			flag = true;
			cameraPos[0] = x1;
		}
		if (cameraPos[0] > x2)
		{
			flag = true;
			cameraPos[0] = x2;
		}
		if (cameraPos[2] < z1)
		{
			flag = true;
			cameraPos[2] = z1;
		}
		if (cameraPos[2] > z2)
		{
			flag = true;
			cameraPos[2] = z2;
		}

		//重复上述远离两点距离的操作
		if (flag)
		{
			distance = sqrt((cameraPos[0] - targetPos[0]) * (cameraPos[0] - targetPos[0]) + (cameraPos[2] - targetPos[2]) * (cameraPos[2] - targetPos[2]));
			if (distance <= 2.0f)
			{
				targetPos[0] = 2.0f * (targetPos[0] - cameraPos[0]) / distance + cameraPos[0];
				targetPos[2] = 2.0f * (targetPos[2] - cameraPos[2]) / distance + cameraPos[2];
			}
		}
	}

	glm::vec3 velocity;        //垂直方向速度
	glm::vec3 gravity;         //重力加速度
	glm::vec3 accelerUp;       //方向向上的加速度

	glm::vec4 outerBoundary;
	vector<glm::vec3> innerBoundaryMin;    //碰撞器小的x/y/z坐标
	vector<glm::vec3> innerBoundaryMax;    //碰撞器大的x/y/z坐标
};
