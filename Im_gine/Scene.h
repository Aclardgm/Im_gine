#pragma once

#include <vector>
#include "flecs.h"


//using Entity = ecs_entity_t;




struct Scene
{
	flecs::world world;


	flecs::entity buildEntity()
	{
		return world.entity();
	}


};