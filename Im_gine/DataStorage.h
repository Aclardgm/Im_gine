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

	std::map<ID, Mesh> meshes;
	std::map<ID, Model> models;
	std::map<ID, Texture> textures;

	AssetStorageManager() {}
	~AssetStorageManager() {}
public:

	ID AddMesh(Mesh&& mesh)
	{
		ID id = getUID();
		meshes[id] = mesh;
		return id;
	}
	ID AddModel(Model&& model)
	{
		ID id = getUID();
		models[id] = model;
		return id;
	}
	
	ID AddTexture(Texture&& texture)
	{
		ID id = getUID();
		textures[id] = texture;
		return id;
	}


	const std::map<ID, Model>& getModels() const
	{
		const auto& ref = models;
		return ref;
	}


	const std::map<ID, Mesh>& getMeshes() const
	{
		const auto& ref = meshes;
		return ref;
	}
	const std::map<ID, Texture>& getTextures() const
	{
		const auto& ref = textures;
		return ref;
	}

	std::map<ID, Model>& getModels()
	{
		auto& ref = models;
		return ref;
	}


	std::map<ID, Mesh>& getMeshes()
	{
		auto& ref = meshes;
		return ref;
	}
	std::map<ID, Texture>& getTextures()
	{
		auto& ref = textures;
		return ref;
	}

	//Singleton & protections
	AssetStorageManager(const AssetStorageManager&) = delete;
	AssetStorageManager& operator=(const AssetStorageManager&) = delete;

	static AssetStorageManager& Instance() {
		static AssetStorageManager as;
		return as;
	}


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
