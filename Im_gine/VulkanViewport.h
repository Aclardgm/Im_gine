#pragma once

#include "VulkanSwapChain.h"

class VulkanViewport
{

public:
	VulkanViewport() {}
	VulkanViewport(VulkanDevice* device,VkInstance instance) : swapchain(new VulkanSwapChain(device, instance)) {}


	void cleanup();

	VulkanSwapChain* swapchain;
};
