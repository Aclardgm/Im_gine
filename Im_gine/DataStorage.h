#pragma once


#include <vector>
#include <array>
#include <map> 


#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>
#include <list>





class AssetStorageManager
{
private:
	//TODO : Turn these in Unique PTR
	std::map<ID, Mesh> meshes;
	std::map<ID, Model> models;
	std::map<ID, Texture> textures;
	std::map<ID, std::vector<VkDescriptorSet>> descriptors;
	std::map<ID, VkDescriptorSetLayout> descriptorLayouts;
	std::map<ID, VkDescriptorPool> descriptorPool;
	std::map<ID, VkPipeline> pipelines;
	std::map<ID, VkPipelineLayout> pipelineLayouts;


	AssetStorageManager() {}
	~AssetStorageManager() {}
public:

	//Add Mesh to Data and return ID
	ID AddMesh(Mesh&& mesh)
	{
		ID id = getUID();
		meshes[id] = mesh;
		return id;
	}
	//Add Model to Data and return ID
	ID AddModel(Model&& model)
	{
		ID id = getUID();
		models[id] = model;
		return id;
	}
	//Add Texture to Data and return ID
	ID AddTexture(Texture&& texture)
	{
		ID id = getUID();
		textures[id] = texture;
		return id;
	}
	//Add VkDescriptorSet to Data and return ID
	ID AddDescriptorSet(VkDescriptorSet&& desc)
	{
		ID id = getUID();
		descriptors[id].push_back(desc);
		return id;
	}
	//Add VkDescriptorSetLayout to Data and return ID
	ID AddDescriptorSetLayout(VkDescriptorSetLayout&& descLayout)
	{
		ID id = getUID();
		descriptorLayouts[id] = descLayout;
		return id;
	}
	//Add VkDescriptorPool to Data and return ID
	ID AddDescriptorPool(VkDescriptorPool&& descPool)
	{
		ID id = getUID();
		descriptorPool[id] = descPool;
		return id;
	}
	//Add VkPipeline to Data and return ID
	ID AddPipeline(VkPipeline&& pipeline)
	{
		ID id = getUID();
		pipelines[id] = pipeline;
		return id;
	}
	//Add VkPipelineLayout to Data and return ID
	ID AddPipelineLayout(VkPipelineLayout&& pipelineLayout)
	{
		ID id = getUID();
		pipelineLayouts[id] = pipelineLayout;
		return id;
	}


	//Create and Add Mesh to Data and return pair<ID,Mesh>
	std::pair<ID,Mesh&> getMesh()
	{
		ID id = getUID();
		meshes[id] = Mesh();
		return std::pair<ID, Mesh&>(id,meshes[id]);
	}
	//Create and Add Model to Data and return pair<ID,Model>
	std::pair<ID, Model&> getModel()
	{
		ID id = getUID();
		models[id] = Model();
		return std::pair<ID, Model&>(id, models[id]);
	}
	//Create and Add Texture to Data and return pair<ID,Texture>
	std::pair<ID, Texture&> getTexture()
	{
		ID id = getUID();
		textures[id] = Texture();
		return std::pair<ID, Texture&>(id, textures[id]);
	}
	//Create and Add VkDescriptorSet to Data and return pair<ID,VkDescriptorSet>
	std::pair<ID, std::vector<VkDescriptorSet>&> getDescriptorSet()
	{
		ID id = getUID();
		descriptors[id] = std::vector<VkDescriptorSet>();
		return std::pair<ID, std::vector<VkDescriptorSet>&>(id, descriptors[id]);
	}
	//Create and Add VkDescriptorSetLayout to Data and return pair<ID,VkDescriptorSetLayout>
	std::pair<ID, VkDescriptorSetLayout*> getDescriptorLayout()
	{
		ID id = getUID();
		descriptorLayouts[id] = VkDescriptorSetLayout();
		return std::pair<ID, VkDescriptorSetLayout*>(id, &descriptorLayouts[id]);
	}
	//Create and Add VkDescriptorPool to Data and return pair<ID,VkDescriptorPool>
	std::pair<ID, VkDescriptorPool*> getDescriptorPool()
	{
		ID id = getUID();
		descriptorPool[id] = VkDescriptorPool();
		return std::pair<ID, VkDescriptorPool*>(id, &descriptorPool[id]);
	}
	//Create and Add VkPipeline to Data and return pair<ID,VkPipeline>
	std::pair<ID, VkPipeline*> getPipeline()
	{
		ID id = getUID();
		pipelines[id] = VkPipeline();
		return std::pair<ID, VkPipeline*>(id, &(pipelines[id]));
	}
	//Create and Add VkPipelineLayout to Data and return pair<ID,VkPipelineLayout>
	std::pair<ID, VkPipelineLayout*> getPipelineLayout()
	{
		ID id = getUID();
		pipelineLayouts[id] = VkPipelineLayout();
		return std::pair<ID, VkPipelineLayout*>(id, &pipelineLayouts[id]);
	}



	//Return Mesh by ID (throw on OOB) 
	Mesh& getMesh(ID id)
	{
		return meshes.at(id);
	}
	//Return Model by ID (throw on OOB) 
	Model& getModel(ID id)
	{
		return models.at(id);
	}
	//Return Texture by ID (throw on OOB) 
	Texture& getTexture(ID id)
	{
		return textures.at(id);
	}
	//Return VkDescriptorSet by ID (throw on OOB) 
	std::vector<VkDescriptorSet>& getDescriptorSet(ID id)
	{
		return descriptors.at(id);
	}
	//Return VkDescriptorSetLayout by ID (throw on OOB) 
	VkDescriptorSetLayout* getDescriptorLayout(ID id)
	{
		return &descriptorLayouts[id];
	}
	//Return VkDescriptorPool by ID (throw on OOB) 
	VkDescriptorPool* getDescriptorPool(ID id)
	{
		return &descriptorPool[id];
	}
	//Return VkPipeline by ID (throw on OOB) 
	VkPipeline* getPipeline(ID id)
	{
		return &pipelines[id];
	}
	//Return VkPipelineLayout by ID (throw on OOB) 
	VkPipelineLayout* getPipelineLayout(ID id)
	{
		return &pipelineLayouts[id];
	}


	//Return const& of map <ID,Mesh>
	const std::map<ID, Mesh>& getMeshes() const
	{
		const auto& ref = meshes;
		return ref;
	}
	//Return const& of map <ID,Model>
	const std::map<ID, Model>& getModels() const
	{
		const auto& ref = models;
		return ref;
	}
	//Return const& of map <ID,Texture>
	const std::map<ID, Texture>& getTextures() const
	{
		const auto& ref = textures;
		return ref;
	}
	//Return const& of map <ID,VkDescriptorSet>
	const std::map<ID, std::vector<VkDescriptorSet>>& getDescriptorSet() const
	{
		const auto& ref = descriptors;
		return ref;
	}
	//Return const& of map <ID,VkDescriptorSetLayout>
	const std::map<ID, VkDescriptorSetLayout>& getDescriptorSetLayouts() const
	{
		const auto& ref = descriptorLayouts;
		return ref;
	}
	//Return const& of map <ID,VkDescriptorPool>
	const std::map<ID, VkDescriptorPool>& getDescriptorPools() const
	{
		const auto& ref = descriptorPool;
		return ref;
	}
	//Return const& of map <ID,VkPipeline>
	const std::map<ID, VkPipeline>& getPipelines() const
	{
		const auto& ref = pipelines;
		return ref;
	}
	//Return const& of map <ID,VkPipelineLayout>
	const std::map<ID, VkPipelineLayout>& getPipelineLayouts() const
	{
		const auto& ref = pipelineLayouts;
		return ref;
	}


	//Return & of map <ID,Model>
	std::map<ID, Model>& getModels()
	{
		auto& ref = models;
		return ref;
	}
	//Return & of map <ID,Mesh>
	std::map<ID, Mesh>& getMeshes()
	{
		auto& ref = meshes;
		return ref;
	}
	//Return & of map <ID,Texture>
	std::map<ID, Texture>& getTextures()
	{
		auto& ref = textures;
		return ref;
	}
	//Return & of map <ID,VkDescriptorSet>
	std::map<ID, std::vector<VkDescriptorSet>>& getDescriptorSets()
	{
		auto& ref = descriptors;
		return ref;
	}
	//Return & of map <ID,VkDescriptorSetLayout>
	std::map<ID, VkDescriptorSetLayout>& getDescriptorSetLayouts()
	{
		auto& ref = descriptorLayouts;
		return ref;
	}
	//Return & of map <ID,VkDescriptorPool>
	std::map<ID, VkDescriptorPool>& getDescriptorPools()
	{
		auto& ref = descriptorPool;
		return ref;
	}
	//Return & of map <ID,VkPipeline>
	std::map<ID, VkPipeline>& getPipelines()
	{
		auto& ref = pipelines;
		return ref;
	}
	//Return & of map <ID,VkPipelineLayout>
	std::map<ID, VkPipelineLayout>& getPipelineLayouts()
	{
		auto& ref = pipelineLayouts;
		return ref;
	}


	//Singleton & protections
	AssetStorageManager(const AssetStorageManager&) = delete;
	AssetStorageManager& operator=(const AssetStorageManager&) = delete;


	//Return unique instance & of data storage
	static AssetStorageManager& Instance() {
		static AssetStorageManager as;
		return as;
	}


	//Clean all data
	void cleanup(VkDevice& device)
	{
		//Cleaning Textures buffers
		for (auto ite = textures.begin(); ite != textures.end(); ite++)
		{
			Texture& tex = (*ite).second;

			vkDestroySampler(device, tex.textureSampler, nullptr);
			vkDestroyImageView(device, tex.textureImageView, nullptr);

			vkDestroyImage(device, tex.textureImage, nullptr);
			vkFreeMemory(device, tex.textureImageMemory, nullptr);
		}

		//Cleaning Meshes buffers
		for (auto ite = models.begin(); ite != models.end(); ite++)
		{
			Model& model = (*ite).second;

			for (int i = 0; i < model.meshesID.size(); i++)
			{

				vkDestroyBuffer(device, model.indexBuffer[i], nullptr);
				vkFreeMemory(device, model.indexBufferMemory[i], nullptr);

				vkDestroyBuffer(device, model.vertexBuffer[i], nullptr);
				vkFreeMemory(device, model.vertexBufferMemory[i], nullptr);
			}
		}
	}


};
