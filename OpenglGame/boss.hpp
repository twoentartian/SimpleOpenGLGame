#pragma once

#include <glm/glm.hpp>

#include "Scene.hpp"
#include "SceneController.hpp"

/*

			*
			|
			|
			|
			|
			|
		---------
		|		|
		|		|
		|		|
		|		|
		---------

*/

namespace boss_env
{
	const std::string layer1 = "layer1";
	const std::string layer2_1 = "layer2_1";
	const std::string layer2_2 = "layer2_2";
	const std::string layer3 = "layer3";
	const std::string layer4 = "layer4";
	const std::string layer5_1 = "layer5_1";
	const std::string layer5_2 = "layer5_2";
	const std::string layer5_3 = "layer5_3";

	const glm::vec3 start_loction = glm::vec3(0, -1.0f * roomSizeY / 2.0f + 2.5f, 0);
	float layer4_angle = 0;
}

void InitBoss()
{
	//bottom layer1
	sceneBox::Container_Add(boss_env::layer1, boss_env::start_loction, glm::vec3(5, 5, 40));

	//layer2
	auto layer1 = sceneBox::Container_Get(boss_env::layer1);
	auto layer1_mat = layer1->getMappingMatrix3();
	{
		auto new_location1 = layer1_mat * glm::vec3(0, 0, 0.5) + boss_env::start_loction;
		auto new_location2 = layer1_mat * glm::vec3(0, 0, -0.5) + boss_env::start_loction;
		sceneBox::Container_Add(boss_env::layer2_1, new_location1 + glm::vec3(0, 20, 0), glm::vec3(5, 40, 5));
		sceneBox::Container_Add(boss_env::layer2_2, new_location2 + glm::vec3(0, 20, 0), glm::vec3(5, 40, 5));
	}
	
	//layer3
	auto layer2_1 = sceneBox::Container_Get(boss_env::layer2_1);
	auto layer2_2 = sceneBox::Container_Get(boss_env::layer2_2);
	{
		auto new_location = boss_env::start_loction + glm::vec3(0, 40, 0);
		sceneBox::Container_Add(boss_env::layer3, new_location, glm::vec3(5, 5, 40));
	}

	//layer4
	auto layer3 = sceneBox::Container_Get(boss_env::layer3);
	{
		auto layer3_mat = layer3->getMappingMatrix3();
		auto terminal_loc = layer3_mat * glm::vec3(0, 0.5, 0) + layer3->boxPosition;
		auto dir1 = terminal_loc - layer3->boxPosition;
		auto new_location1 = terminal_loc + (normalize(dir1)*=10);
		sceneBox::Container_Add(boss_env::layer4, new_location1, glm::vec3(5, 20, 5));
	}

	//layer5
	auto layer4 = sceneBox::Container_Get(boss_env::layer4);
	{
		auto layer4_mat = layer4->getMappingMatrix3();
		auto new_location1 = layer4_mat * glm::vec3(0, 0.5, 0) + layer4->boxPosition;
		sceneBox::Container_Add(boss_env::layer5_1, new_location1, glm::vec3(16, 5, 5));
		sceneBox::Container_Add(boss_env::layer5_2, new_location1, glm::vec3(5, 16, 5));
		sceneBox::Container_Add(boss_env::layer5_3, new_location1, glm::vec3(5, 5, 16));
	}
	
}

void drawBoss()
{
	//bottom layer1
	auto layer1 = sceneBox::Container_Get(boss_env::layer1);
	{
		layer1->boxRotateAxis = glm::vec3(0, 1, 0);
		layer1->boxRotateRadius += 0.5;
	}

	//layer2

	auto layer2_1 = sceneBox::Container_Get(boss_env::layer2_1);
	auto layer2_2 = sceneBox::Container_Get(boss_env::layer2_2);
	{
		auto layer_mat = layer1->getMappingMatrix3();
		auto new_location1 = layer_mat * glm::vec3(0, 0, 0.5);
		auto new_location2 = layer_mat * glm::vec3(0, 0, -0.5);
		layer2_1->boxPosition = new_location1 + boss_env::start_loction + glm::vec3(0, 20, 0);
		layer2_2->boxPosition = new_location2 + boss_env::start_loction + glm::vec3(0, 20, 0);
		layer2_1->boxRotateAxis = glm::vec3(0, 1, 0);
		layer2_2->boxRotateAxis = glm::vec3(0, 1, 0);
		layer2_1->boxRotateRadius += 0.5;
		layer2_2->boxRotateRadius += 0.5;
	}

	//layer3
	auto layer3 = sceneBox::Container_Get(boss_env::layer3);
	{
		layer3->boxRotateAxis = glm::vec3(0, 1, 0);
		layer3->boxRotateRadius += 0.5;
		layer3->boxRotateAxis2 = glm::vec3(0, 0, 1);
		layer3->boxRotateRadius2 += 0.25;
	}

	//layer4
	auto layer4 = sceneBox::Container_Get(boss_env::layer4);
	{
		auto layer3_mat = layer3->getMappingMatrix3();
		auto terminal_loc = layer3_mat * glm::vec3(0, 0.5, 0) + layer3->boxPosition;
		auto dir1 = terminal_loc - layer3->boxPosition;
		dir1 = glm::normalize(dir1);
		auto new_location1 = terminal_loc + (dir1 *= 10);
		layer4->boxPosition = new_location1;
		layer4->boxRotateAxis = glm::vec3(0, 1, 0);
		layer4->boxRotateRadius += 0.5;
		layer4->boxRotateAxis2 = glm::vec3(0, 0, 1);
		layer4->boxRotateRadius2 += 0.25;
	}

	//layer5
	auto layer5_1 = sceneBox::Container_Get(boss_env::layer5_1);
	auto layer5_2 = sceneBox::Container_Get(boss_env::layer5_2);
	auto layer5_3 = sceneBox::Container_Get(boss_env::layer5_3);
	{
		auto layer4_mat = layer4->getMappingMatrix3();
		auto new_location1 = layer4_mat * glm::vec3(0, 0.5, 0) + layer4->boxPosition;
		layer5_1->boxPosition = layer5_2->boxPosition = layer5_3->boxPosition = new_location1;
		layer5_1->boxRotateAxis = glm::vec3(1, 1, 1);
		layer5_1->boxRotateRadius += 0.5;
		layer5_2->boxRotateAxis = layer5_3->boxRotateAxis = layer5_1->boxRotateAxis;
		layer5_2->boxRotateRadius = layer5_3->boxRotateRadius = layer5_1->boxRotateRadius;
		
	}


}