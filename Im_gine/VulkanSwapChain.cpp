#include "VulkanSwapChain.h"
#include "VulkanRHI.h"
#include "Helper.h"

#include <array>



VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, VkInstance instance)
    : device(device),
    instance(instance)
{



}

SwapChainSupportDetails VulkanSwapChain::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}


void VulkanSwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device->physicalDevice);

    VkSurfaceFormatKHR surfaceFormat =  vkh::chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = vkh::chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = vkh::chooseSwapExtent(device->rhi->window.window, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device->findQueueFamilies(device->physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device->device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device->device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device->device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void VulkanSwapChain::createSurface() {
    if (glfwCreateWindowSurface(instance, device->rhi->window.window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}


void VulkanSwapChain::createColorResources() {
    VkFormat colorFormat = swapChainImageFormat;

    vkh::createImage(*device, swapChainExtent.width, swapChainExtent.height, 1,device->msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
    colorImageView = vkh::createImageView(device->device,colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void VulkanSwapChain::createDepthResources() {
    VkFormat depthFormat = vkh::findDepthFormat(device->physicalDevice);

    vkh::createImage(*device,swapChainExtent.width, swapChainExtent.height, 1, device->msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    depthImageView = vkh::createImageView(device->device,depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}


void VulkanSwapChain::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = vkh::createImageView(device->device,swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

void VulkanSwapChain::createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            colorImageView,
            depthImageView,
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = device->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device->device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}


void VulkanSwapChain::recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(device->rhi->window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(device->rhi->window.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device->device);



    device->rhi->viewport.swapchain->cleanup();
    device->rhi->viewport.swapchain->createSwapChain();
    device->rhi->viewport.swapchain->createImageViews();
    device->rhi->viewport.swapchain->createColorResources();
    device->rhi->viewport.swapchain->createDepthResources();
    device->rhi->viewport.swapchain->createFramebuffers();

}

void VulkanSwapChain::cleanup() {
    vkDestroyImageView(device->device, depthImageView, nullptr);
    vkDestroyImage(device->device, depthImage, nullptr);
    vkFreeMemory(device->device, depthImageMemory, nullptr);

    vkDestroyImageView(device->device, colorImageView, nullptr);
    vkDestroyImage(device->device, colorImage, nullptr);
    vkFreeMemory(device->device, colorImageMemory, nullptr);

    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device->device, framebuffer, nullptr);
    }

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device->device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device->device, swapChain, nullptr);
}