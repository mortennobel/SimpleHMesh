//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>


class Halfedge;
class HMesh;

class Vertex {
public:
    Vertex(HMesh* hMesh):hMesh{hMesh}{}
    Vertex(const Vertex&) = delete;
    ~Vertex();

    Halfedge* halfedge = nullptr;
    glm::vec3 position = glm::vec3{0};

    void replaceVertex(Vertex* newVertex);

    bool isBoundary();

    std::vector<Halfedge*> circulate();

    std::vector<Halfedge*> circulateOpp();

    std::vector<Halfedge*> circulateOneRing();

    bool isValid();
private:


    HMesh* hMesh;

    friend class HMesh;
};

inline std::ostream& operator<<(std::ostream& os, Vertex *dt)
{
    os << "Vertex{ id: "<< (void*)dt << ",halfedge:" << (void*)dt->halfedge << ",position:" << glm::to_string(dt->position) << "}";
    return os;
}


