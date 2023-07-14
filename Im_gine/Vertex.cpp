#include "Vertex.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>





bool Vertex::operator==(const Vertex& other) const {
    return pos == other.pos && color == other.color && texCoord == other.texCoord;
}