#pragma once

#include <vector> 

#define DATASIZE_MESH (4 * DATASIZE_ID)

struct Mesh
{
public:

    Mesh() : vertices(),indices() {}
    Mesh(std::vector<Vertex>& vert, std::vector<uint32_t>& ind) : vertices(vert), indices(ind) {}
    Mesh(std::vector<Vertex>&& vert, std::vector<uint32_t>&& ind) : vertices(std::move(vert)), indices(std::move(ind)) {}
    //Mesh(const Mesh& m) :vertices(m.vertices),indices(m.indices) {}


    


    //Mesh& operator=(const Mesh& m) 
    //{
    //    vertices = m.vertices;
    //    indices = m.indices;
    //    return *this;
    //}


    //ID beginVertices;   //4 bytes
    //ID endVertices;     //4 bytes
    //ID beginIndices;    //4 bytes
    //ID endIndices;      //4 bytes


    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};