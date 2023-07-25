#pragma once

#include "glminclude.h"
#include <vulkan/vulkan.h>
#include <vector>


#include "VulkanDevice.h"

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


class VulkanSwapChain
{
public:

	VulkanSwapChain(VulkanDevice* device, VkInstance instance);

	void createSwapChain();
	void createImageViews();
	void createSurface();
	void createColorResources();
	void createDepthResources();
	void createFramebuffers();
	void recreateSwapChain();


	void cleanup();

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);


	VulkanDevice* device;
	VkInstance instance;

	VkSwapchainKHR swapChain;
	VkSurfaceKHR surface;



	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;



	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
};