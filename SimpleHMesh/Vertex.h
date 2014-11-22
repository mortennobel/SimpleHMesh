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
    Vertex(HMesh* hMesh);
    Vertex(const Vertex&) = delete;
    ~Vertex();

    Halfedge* halfedge = nullptr;
    glm::vec3 position = glm::vec3{0};

    void replaceVertex(Vertex* newVertex);

    bool isBoundary();

    std::vector<Halfedge*> circulate();

    std::vector<Halfedge*> circulateOpp();

    std::vector<Halfedge*> circulateOneRing();

    // dissolves a vertex. Note the vertex must only have two outgoing edges.
    void dissolve();

    bool isValid();
#ifdef DEBUG
    int id;
#endif



private:


    HMesh* hMesh;

    friend class HMesh;
};

std::ostream & operator<<(std::ostream& os, Vertex *dt);


