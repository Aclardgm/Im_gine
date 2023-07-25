#include "VulkanViewport.h"

void VulkanViewport::cleanup()
{
	swapchain->cleanup();
}
