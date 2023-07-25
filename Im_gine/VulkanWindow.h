#pragma once
#include "Window.h"

class GLFWwindow;

class VulkanWindow : public Window
{
public:
	virtual void getFrameBufferSize(int*, int*) override;


	void initWindow() override;


	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	bool framebufferResized = false;
	GLFWwindow* window;

	// Hérité via Window
	virtual bool shouldClose() override;
	virtual void events() override;
	virtual void close() override;
};