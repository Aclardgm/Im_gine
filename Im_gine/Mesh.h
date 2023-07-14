#pragma once

#include <vector> 

#define DATASIZE_MESH (4 * DATASIZE_ID)

struct Mesh
{
public:

    Mesh() : vertices(),indices() {}
    Mesh(std::vector<Vertex>& vert, std::vector<uint32_t>& ind) : vertices(vert), indices(ind) {}
    Mesh(std::vector<Vertex>&& vert, std::vector<uint32_t>&& ind) : vertices(std::move(vert)), indices(std::move(ind)) {}
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};