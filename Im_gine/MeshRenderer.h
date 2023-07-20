#pragma once


#include <vector> 

#include "IdManager.h"



struct MeshRenderer
{
	ID modelID;
	ID textureID;
	ID pipelineID;
	ID pipelineLayoutID;
	ID descriptorSetID;
	ID descriptorSetLayoutID;
	ID descriptorPoolID;
};