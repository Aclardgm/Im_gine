#pragma once



namespace vkh
{

	std::vector<char> readFile(const std::string& filename);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance, 
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	/*void DestroyDebugUtilsMessengerEXT(
		VkInstance instance, 
		VkDebugUtilsMessengerEXT debugMessenger, 
		const VkAllocationCallbacks* pAllocator);*/
	VkShaderModule createShaderModule(VkDevice* device, const std::vector<char>& code);
	void createRenderPass(
		VkDevice device,
		VkFormat swapChainImageFormat,
		VkSampleCountFlagBits msaaSamples,
		VkFormat depthFormat,
		VkRenderPass* renderPass
	);
	void createImage(
		VulkanDevice& device, 
		uint32_t width, 
		uint32_t height, 
		uint32_t mipLevels, 
		VkSampleCountFlagBits numSamples, 
		VkFormat format, 
		VkImageTiling tiling, 
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkImage& image, 
		VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);




	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice);
	VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
	VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
	void endSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandBuffer commandBuffer, VkCommandPool commandPool);

	void createDescriptorSetLayout_UBO_SAMPLER(VkDevice device, VkDescriptorSetLayout** layout);
	void createDescriptorPool_UBO_SAMPLER(VkDevice device, VkDescriptorPool* pool, int flightFramesCount);
	void createGraphicPipeline_VERT_FRAG(
		const std::string& vertShaderPath,
		const std::string& fragShaderPath,
		VkDevice* device,
		VkDescriptorSetLayout* descriptorLayout,
		VkPipeline* pipeline,
		VkPipelineLayout* pipelineLayout,
		VkSampleCountFlagBits msaaSamples,
		VkRenderPass pass);
	void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void createBuffer(
		VkDevice device,
		VkDeviceSize size,
		VkPhysicalDevice physicalDevice,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);
	void createCommandBuffers(
		VkDevice device,
		std::vector<VkCommandBuffer>& commandBuffers,
		VkCommandPool commandPool,
		int flightFramesCount);
	void createSyncObjects(
		VkDevice device,
		std::vector<VkSemaphore>& imageAvailableSemaphores,
		std::vector<VkSemaphore>& renderFinishedSemaphores,
		std::vector<VkFence>& inFlightFences,
		int flightFramesCount);
		template<typename UBO>
		void createUniformBuffers(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			std::vector<VkBuffer>& uniformBuffers,
			std::vector<VkDeviceMemory>& uniformBuffersMemory,
			std::vector<void*>& uniformBuffersMapped,
			int flightFramesCount);
}
