#pragma once


#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include "IdManager.h"

class VulkanRHI;

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


class VulkanDevice
{
	

public:
	VulkanDevice() = delete;
	VulkanDevice(VulkanRHI* rhi) : rhi(rhi) {}


	~VulkanDevice()
	{
        
	}


	void init();


	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	VkSampleCountFlagBits getMaxUsableSampleCount();
	void createLogicalDevice();
	void createCommandPool();

	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	void cleanup();

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
    VulkanRHI* rhi;
	VkCommandPool commandPool;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkRenderPass renderPass;

	ID pipelineLayoutID;
	ID pipelineID;
	ID descriptorSetLayoutID;

	ID uniformBuffers;
	ID uniformBuffersMemory;
	ID uniformBuffersMapped;
	ID commandBuffers;
	ID imageAvailableSemaphores;
	ID renderFinishedSemaphores;
	ID inFlightFences;
};