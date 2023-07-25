#include "VulkanWindow.h"
#include "Debug.h"
#include "glminclude.h"




void VulkanWindow::getFrameBufferSize(int* width, int* height)
{
    glfwGetFramebufferSize(window, width, height);
}

void VulkanWindow::initWindow() {

    LOG_INFO("Started Init glfw Window")

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void VulkanWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

bool VulkanWindow::shouldClose()
{
    return !glfwWindowShouldClose(window);
}

void VulkanWindow::events()
{
    glfwPollEvents();
}

void VulkanWindow::close()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}


