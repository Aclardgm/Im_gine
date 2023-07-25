#pragma once

#include <cstdint>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Window
{
	virtual void getFrameBufferSize(int* ,int*) = 0;
	virtual void initWindow() = 0;
	virtual bool shouldClose() = 0;
	virtual void events() = 0;
	virtual void close() = 0;

};


