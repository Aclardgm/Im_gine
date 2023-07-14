#pragma once

#include <vector> 



struct Model
{
    std::vector<ID> meshesID;
    std::vector<VkBuffer> vertexBuffer;
    std::vector<VkDeviceMemory> vertexBufferMemory;
    std::vector<VkBuffer> indexBuffer;
    std::vector<VkDeviceMemory> indexBufferMemory;
};