#pragma once

#include <vector>
#include "flecs.h"



struct Scene
{
	flecs::world world;


	flecs::entity buildEntity()
	{
		return world.entity();
	}


};