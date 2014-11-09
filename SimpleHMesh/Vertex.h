//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#pragma once

#include <vector>
#include <glm/glm.hpp>


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



