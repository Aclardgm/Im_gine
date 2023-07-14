#pragma once

#include "Scene.h"
#include "DataStorage.h"

#include <optional>
#include <memory>





class World
{
private:
	std::optional<Scene*> currentScene;

	std::vector<Scene> scenes;



	
public:

	World()
	{
		currentScene.reset();
		scenes.clear();
	}


	Scene* getCurrentScene()
	{
		return currentScene.value();
	}

	Scene* createScene()
	{
		scenes.push_back(Scene());
		if (!currentScene.has_value())
		{
			currentScene = &scenes[scenes.size() - 1];
		}

		return &scenes[scenes.size() - 1];
	}
};