

#include "glminclude.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <array>
#include <optional>-
#include <set>
#include <unordered_map>
#include <memory>


#include "Debug.h"
#include "DataStorage.h"

#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Vertex.h"
#include "Texture.h"
#include "Mesh.h"
#include "Model.h"
#include "Scene.h"
#include "World.h"


#include "VulkanRHI.h"

#include "Helper.h"
#include "Helper.cpp"






const int MAX_FRAMES_IN_FLIGHT = 2;





struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class Application {
public:
    void run() {
        
        //initWindow();
        initWorld();
        vulkanRHI.init();
        initVulkan();
        buildCamera();
        mainLoop();
        cleanup();
    }

private:

    uint32_t mipLevels;
    
    uint32_t currentFrame = 0;



    std::unique_ptr<World> world;

    Scene* mainScene;

    bool framebufferResized = false;

    struct Context
    {
        ID descriptorPoolID_1;
        ID descriptorPoolID_2;
    };


    Context basicContext;


    VulkanRHI vulkanRHI;



    void initWorld()
    {
        world = std::make_unique<World>();

        mainScene = world.get()->createScene();
    }



    void buildCamera()
    {
        glm::vec3 camPos(10.0f, 10.0f, 10.0f);

        mainScene->buildEntity()
            .set<Position>({ camPos })
            .set<Camera>({
                glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),glm::perspective(glm::radians(45.0f), //Camera LookAt
                vulkanRHI.viewport.swapchain->swapChainExtent.width / (float)vulkanRHI.viewport.swapchain->swapChainExtent.height, 0.1f, 10.0f) //Camera projection
        });
    }


    void initVulkan() {
        LOG_INFO("Started Init Vulkan")

        ASSETSTORAGE;

        vulkanRHI.device->createCommandPool();

        vulkanRHI.viewport.swapchain->createColorResources();
        vulkanRHI.viewport.swapchain->createDepthResources();
        vulkanRHI.viewport.swapchain->createFramebuffers();


        loadAssets();

        createAllVertexBuffers();
        createAllIndexBuffers();


        auto uniformBuffers = am.getUniformBuffer();
        auto uniformBuffersMemory = am.getUniformBufferMemory();
        auto uniformBuffersMapped = am.getUniformBufferMapped();

        vulkanRHI.device->uniformBuffers = uniformBuffers.first;
        vulkanRHI.device->uniformBuffersMemory = uniformBuffersMemory.first;
        vulkanRHI.device->uniformBuffersMapped = uniformBuffersMapped.first;

        vkh::createUniformBuffers<UniformBufferObject>(
            vulkanRHI.device->device,
            vulkanRHI.device->physicalDevice,
            uniformBuffers.second, 
            uniformBuffersMemory.second,
            uniformBuffersMapped.second,
            MAX_FRAMES_IN_FLIGHT);

        auto pool_1 = am.getDescriptorPool();
        basicContext.descriptorPoolID_1 = pool_1.first;
        auto pool_2 = am.getDescriptorPool();
        basicContext.descriptorPoolID_2 = pool_2.first;

        vkh::createDescriptorPool_UBO_SAMPLER(vulkanRHI.device->device, pool_1.second, MAX_FRAMES_IN_FLIGHT);
        vkh::createDescriptorPool_UBO_SAMPLER(vulkanRHI.device->device, pool_2.second, MAX_FRAMES_IN_FLIGHT);


        Scene* currentScene = world.get()->getCurrentScene();
        flecs::query<MeshRenderer> q = currentScene->world.query<MeshRenderer>();

        bool test = 0;

        q.each([this, &am,&test]
        (flecs::entity e, MeshRenderer& rend) {
            if (test == 0)
            {
                rend.descriptorPoolID = basicContext.descriptorPoolID_1;

            }
            else
            {
                rend.descriptorPoolID = basicContext.descriptorPoolID_2;

            }
            test = !test;
            rend.descriptorSetLayoutID = vulkanRHI.device->descriptorSetLayoutID;
            rend.pipelineID = vulkanRHI.device->pipelineID;
            rend.pipelineLayoutID = vulkanRHI.device->pipelineLayoutID;
        });

        createAllDescriptorSets();


        auto commandBuffers = am.getCommandBuffer();
        auto imageAvailableSemaphores = am.getAvailableSemaphore();
        auto renderFinishedSemaphores = am.getFinishedSemaphore();
        auto inFlightFences = am.getInFlightFence();

        vulkanRHI.device->commandBuffers = commandBuffers.first;
        vulkanRHI.device->imageAvailableSemaphores = imageAvailableSemaphores.first;
        vulkanRHI.device->renderFinishedSemaphores = renderFinishedSemaphores.first;
        vulkanRHI.device->inFlightFences = inFlightFences.first;

        vkh::createCommandBuffers(vulkanRHI.device->device, commandBuffers.second, vulkanRHI.device->commandPool, MAX_FRAMES_IN_FLIGHT);
        vkh::createSyncObjects(vulkanRHI.device->device, imageAvailableSemaphores.second, renderFinishedSemaphores.second, inFlightFences.second, MAX_FRAMES_IN_FLIGHT);
        vkh::createSyncObjects(vulkanRHI.device->device, imageAvailableSemaphores.second, renderFinishedSemaphores.second, inFlightFences.second, MAX_FRAMES_IN_FLIGHT);
    }

    void mainLoop() {
        while (vulkanRHI.window.shouldClose()) {
            vulkanRHI.window.events();
            drawFrame();
        }

        vkDeviceWaitIdle(vulkanRHI.device->device);
    }

    //void cleanupSwapChain() {
    //    vkDestroyImageView(device, depthImageView, nullptr);
    //    vkDestroyImage(device, depthImage, nullptr);
    //    vkFreeMemory(device, depthImageMemory, nullptr);

    //    vkDestroyImageView(device, colorImageView, nullptr);
    //    vkDestroyImage(device, colorImage, nullptr);
    //    vkFreeMemory(device, colorImageMemory, nullptr);

    //    for (auto framebuffer : swapChainFramebuffers) {
    //        vkDestroyFramebuffer(device, framebuffer, nullptr);
    //    }

    //    for (auto imageView : swapChainImageViews) {
    //        vkDestroyImageView(device, imageView, nullptr);
    //    }

    //    vkDestroySwapchainKHR(device, swapChain, nullptr);
    //}

    void cleanup() {
        vulkanRHI.viewport.swapchain->cleanup();


        vulkanRHI.shutdown();

        //vkDestroyPipeline(device, graphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        //vkDestroyRenderPass(vulkanRHI.device->device, renderPass, nullptr);

        /*for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(vulkanRHI.device->device, uniformBuffers[i], nullptr);
            vkFreeMemory(vulkanRHI.device->device, uniformBuffersMemory[i], nullptr);
        }*/

        //vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        
        
        AssetStorageManager::Instance().cleanup(vulkanRHI.device->device);
        
        //vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);


        /*for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(vulkanRHI.device->device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(vulkanRHI.device->device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(vulkanRHI.device->device, inFlightFences[i], nullptr);
        }*/



        //vkDestroyCommandPool(vulkanRHI.device->device, vulkanRHI.device->commandPool, nullptr);

        //vkDestroyDevice(device, nullptr);

        if (vulkanRHI.enableValidationLayers) {
            vulkanRHI.DestroyDebugUtilsMessengerEXT(vulkanRHI.instance, vulkanRHI.debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(vulkanRHI.instance, vulkanRHI.viewport.swapchain->surface, nullptr);
        vkDestroyInstance(vulkanRHI.instance, nullptr);

        /*glfwDestroyWindow(window);

        glfwTerminate();*/
        

    }


    void createDescriptorPools()
    {
        ASSETSTORAGE;
        for (auto model = am.getModels().begin();model != am.getModels().end();model++)
        {
            auto pool = am.getDescriptorPool();
            vkh::createDescriptorPool_UBO_SAMPLER(vulkanRHI.device->device, pool.second, MAX_FRAMES_IN_FLIGHT);
        }
    }

    bool hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    Texture createTextureImage(std::string& path) {
        int texWidth, texHeight, texChannels;
        Texture tex;
        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        vkh::createBuffer(vulkanRHI.device->device,imageSize, vulkanRHI.device->physicalDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(vulkanRHI.device->device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(vulkanRHI.device->device, stagingBufferMemory);

        stbi_image_free(pixels);

        vkh::createImage(*vulkanRHI.device, texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, tex.textureImage, tex.textureImageMemory);

        transitionImageLayout(tex.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
        copyBufferToImage(stagingBuffer, tex.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

        vkDestroyBuffer(vulkanRHI.device->device, stagingBuffer, nullptr);
        vkFreeMemory(vulkanRHI.device->device, stagingBufferMemory, nullptr);

        vulkanRHI.device->generateMipmaps(tex.textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);

        return tex;
    }


    VkSampleCountFlagBits getMaxUsableSampleCount() {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(vulkanRHI.device->physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    void createTextureImageView(Texture& tex) {
        tex.textureImageView = vkh::createImageView(vulkanRHI.device->device, tex.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
    }

    void createTextureSampler(Texture& tex) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(vulkanRHI.device->physicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(vulkanRHI.device->device, &samplerInfo, nullptr, &tex.textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }


    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
        VkCommandBuffer commandBuffer = vkh::beginSingleTimeCommands(vulkanRHI.device->device, vulkanRHI.device->commandPool);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        vkh::endSingleTimeCommands(vulkanRHI.device->device, vulkanRHI.device->graphicsQueue, commandBuffer, vulkanRHI.device->commandPool);
    }

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
        VkCommandBuffer commandBuffer = vkh::beginSingleTimeCommands(vulkanRHI.device->device, vulkanRHI.device->commandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        vkh::endSingleTimeCommands(vulkanRHI.device->device, vulkanRHI.device->graphicsQueue, commandBuffer, vulkanRHI.device->commandPool);
    }


    void loadAssets()
    {

        std::vector<std::string> modelPaths= {
            "models/viking_room.obj",
            "models/skull.obj"
            //"models/Cat.obj"
        };

        std::vector<std::string> texturePaths = {
            "textures/viking_room.png",
            "textures/skull.jpg"
            //"textures/Cat_diffuse.jpg"
        };


        AssetStorageManager& am = AssetStorageManager::Instance();

        for (size_t i = 0; i < modelPaths.size(); i++)
        {
            Model model;
            loadAssetAssimp(modelPaths[i], &model);

            Texture tex = createTextureImage(texturePaths[i]);
            createTextureImageView(tex);
            createTextureSampler(tex);

            mainScene->buildEntity().set<MeshRenderer>({ 
                am.AddModel(std::move(model)),
                am.AddTexture(std::move(tex)),
                });
        }
    }



    bool loadAssetAssimp(std::string path,Model* model)
    {
        // Create an instance of the Importer class
        Assimp::Importer importer;

        // And have it read the given file with some example postprocessing
        // Usually - if speed is not the most important aspect for you - you'll
        // probably to request more postprocessing than we do in this example.
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

        // If the import failed, report it
        if (nullptr == scene) {
            LOG_ERROR(importer.GetErrorString())
            return false;
        }


        // Now we can access the file's contents.
        processScene(scene,model);

        // We're done. Everything will be cleaned up by the importer destructor
        return true;
    }


    void processScene(const aiScene* scene,Model* model)
    {

        AssetStorageManager& am = AssetStorageManager::Instance();


        for (size_t i = 0; i < scene->mNumMeshes; i++)
        {

            aiMesh* mesh = scene->mMeshes[i];
            ID id = am.AddMesh(std::move(processMesh(mesh)));
            model->meshesID.push_back(id);
        }
    }

    Mesh processMesh(aiMesh* mesh)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.pos = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).


                vec.x = mesh->mTextureCoords[0][i].x;
                //flipping uv for obj->vulkan texCoord representation
                vec.y = 1.0f - mesh->mTextureCoords[0][i].y;



                vertex.texCoord = vec;
                //// tangent
                //vector.x = mesh->mTangents[i].x;
                //vector.y = mesh->mTangents[i].y;
                //vector.z = mesh->mTangents[i].z;
                //vertex.tangent = vector;
                //// bitangent
                //vector.x = mesh->mBitangents[i].x;
                //vector.y = mesh->mBitangents[i].y;
                //vector.z = mesh->mBitangents[i].z;
                //vertex.bitangent = vector;
            }
            else
                vertex.texCoord = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }



        // return a mesh object created from the extracted mesh data
        return Mesh(std::move(vertices), std::move(indices));
    }


    void createAllVertexBuffers() {

        Scene* currentScene = world.get()->getCurrentScene();


        AssetStorageManager& am = AssetStorageManager::Instance();
        
        flecs::query<MeshRenderer> q = currentScene->world.query<MeshRenderer>();

        

        q.each([this,&am](flecs::entity e, MeshRenderer& rend) {
            Model& model = am.getModels().at(rend.modelID);

            createEntitiesVertexBuffers(rend, vulkanRHI.device->device, vulkanRHI.device->physicalDevice, vulkanRHI.device->commandPool, vulkanRHI.device->graphicsQueue );

        });
    }

    void createAllIndexBuffers() {
        Scene* currentScene = world.get()->getCurrentScene();
        AssetStorageManager& am = AssetStorageManager::Instance();

        flecs::query<MeshRenderer> q = currentScene->world.query<MeshRenderer>();
        q.each([this,&am](flecs::entity e, MeshRenderer& rend) {

            Model& model = am.getModels().at(rend.modelID);

            createEntitiesIndexBuffers(rend, vulkanRHI.device->device, vulkanRHI.device->physicalDevice, vulkanRHI.device->commandPool, vulkanRHI.device->graphicsQueue);

        });

    }


    template<typename UBO>
    void createEntitiesDescriptorSets_UBO_SAMPLER(MeshRenderer& rend, VkDevice device, std::vector<VkBuffer>& uniformBuffers, int flightFramesCount)
    {
        AssetStorageManager& am = AssetStorageManager::Instance();
        std::vector<VkDescriptorSetLayout> layouts(flightFramesCount, *am.getDescriptorLayout(rend.descriptorSetLayoutID));

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = *am.getDescriptorPool(rend.descriptorPoolID);
        allocInfo.descriptorSetCount = static_cast<uint32_t>(flightFramesCount);
        allocInfo.pSetLayouts = layouts.data();

        auto set = am.getDescriptorSet();

        set.second.resize(flightFramesCount);
        rend.descriptorSetID = set.first;


        VkResult res = vkAllocateDescriptorSets(device, &allocInfo, set.second.data());

        if (res != VK_SUCCESS) {
            std::cout << res << std::endl;
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        Texture& tex = am.getTextures()[rend.textureID];

        for (size_t i = 0; i < flightFramesCount; i++) {

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UBO);


            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = tex.textureImageView;
            imageInfo.sampler = tex.textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = set.second[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = set.second[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void createEntitiesVertexBuffers(MeshRenderer& rend, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
    {
        AssetStorageManager& am = AssetStorageManager::Instance();
        Model& model = am.getModels().at(rend.modelID);


        int vertexBufferID = 0;

        for (auto ite = model.meshesID.begin(); ite != model.meshesID.end(); ite++)
        {
            Mesh& mesh = am.getMeshes().at(*ite);

            VkDeviceSize bufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            vkh::createBuffer(device, bufferSize, physicalDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, mesh.vertices.data(), (size_t)bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);


            model.vertexBuffer.push_back(VkBuffer());
            model.vertexBufferMemory.push_back(VkDeviceMemory());

            vkh::createBuffer(device, bufferSize, physicalDevice, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.vertexBuffer[vertexBufferID], model.vertexBufferMemory[vertexBufferID]);


            vkh::copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, model.vertexBuffer[vertexBufferID], bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);


            vertexBufferID++;
        }
    }
    void createEntitiesIndexBuffers(MeshRenderer& rend, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
    {
        AssetStorageManager& am = AssetStorageManager::Instance();
        Model& model = am.getModels().at(rend.modelID);


        int indexBufferID = 0;

        for (auto ite = model.meshesID.begin(); ite != model.meshesID.end(); ite++)
        {

            Mesh& mesh = am.getMeshes().at(*ite);

            VkDeviceSize bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            vkh::createBuffer(device, bufferSize, physicalDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, mesh.indices.data(), (size_t)bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);


            model.indexBuffer.push_back(VkBuffer());
            model.indexBufferMemory.push_back(VkDeviceMemory());

            vkh::createBuffer(device, bufferSize, physicalDevice, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.indexBuffer[indexBufferID], model.indexBufferMemory[indexBufferID]);

            vkh::copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, model.indexBuffer[indexBufferID], bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);

            indexBufferID++;
        }
    }



    void createAllDescriptorSets() {
        ASSETSTORAGE;
        Scene* currentScene = world.get()->getCurrentScene();


        flecs::query<MeshRenderer> q = currentScene->world.query<MeshRenderer>();
        q.each([this, &am, &currentScene](flecs::entity e, MeshRenderer& rend) {
            auto uniformBuffers = am.getUniformBuffer(vulkanRHI.device->uniformBuffers);
            createEntitiesDescriptorSets_UBO_SAMPLER<UniformBufferObject>(rend, vulkanRHI.device->device, uniformBuffers, MAX_FRAMES_IN_FLIGHT);
        });
    }

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vulkanRHI.device->renderPass;
        renderPassInfo.framebuffer = vulkanRHI.viewport.swapchain->swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = vulkanRHI.viewport.swapchain->swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);



        Scene* currentScene = world.get()->getCurrentScene();


        AssetStorageManager& am = AssetStorageManager::Instance();


        flecs::query<MeshRenderer> q = currentScene->world.query<MeshRenderer>();
        q.each([this, &commandBuffer,&currentScene,&am](flecs::entity e, MeshRenderer& rend) {

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *am.getPipeline(rend.pipelineID));

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)vulkanRHI.viewport.swapchain->swapChainExtent.width;
            viewport.height = (float)vulkanRHI.viewport.swapchain->swapChainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = vulkanRHI.viewport.swapchain->swapChainExtent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


            Model& model = am.getModels().at(rend.modelID);


            int indexBuffer = 0;


            VkBuffer* vertexBuffers = new VkBuffer[model.vertexBuffer.size()];

            size_t indicesCount = 0;


            for (auto ite = model.meshesID.begin(); ite != model.meshesID.end(); ite++)
            {
                Mesh& mesh = am.getMeshes().at(*ite);


                VkBuffer vertexBuffers[] = { model.vertexBuffer[indexBuffer]};
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffer, model.indexBuffer[indexBuffer], 0, VK_INDEX_TYPE_UINT32);


                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *am.getPipelineLayout(rend.pipelineLayoutID), 0, 1, &am.getDescriptorSet(rend.descriptorSetID)[currentFrame], 0, nullptr);
                vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
                indexBuffer++;
            }
        });



        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


        Scene* currentScene = world.get()->getCurrentScene();
        
        
        UniformBufferObject ubo{};

        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        flecs::query<const Position,const Camera> q = currentScene->world.query <const Position,const Camera> ();
        q.each([this, &ubo,time](flecs::entity e, const Position& p, const Camera& c) {

            ubo.view = c.view;
            ubo.proj = c.proj;
            ubo.proj[1][1] *= -1;
        });
        
        ASSETSTORAGE;

        auto uniformBuffersMapped = am.getUniformBufferMapped(vulkanRHI.device->uniformBuffersMapped);

        memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    void drawFrame() {

        ASSETSTORAGE;

        auto inFlightFences = am.getInFlightFence(vulkanRHI.device->inFlightFences);
        auto imageAvailableSemaphores = am.getAvailableSemaphore(vulkanRHI.device->imageAvailableSemaphores);
        auto commandBuffers = am.getCommandBuffer(vulkanRHI.device->commandBuffers);
        auto renderFinishedSemaphores = am.getFinishedSemaphore(vulkanRHI.device->renderFinishedSemaphores);

        vkWaitForFences(vulkanRHI.device->device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vulkanRHI.device->device, vulkanRHI.viewport.swapchain->swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            vulkanRHI.viewport.swapchain->recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(currentFrame);

        vkResetFences(vulkanRHI.device->device, 1, &inFlightFences[currentFrame]);

        vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(vulkanRHI.device->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { vulkanRHI.viewport.swapchain->swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(vulkanRHI.device->presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            vulkanRHI.viewport.swapchain->recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }


};

int main() {
    Application app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
