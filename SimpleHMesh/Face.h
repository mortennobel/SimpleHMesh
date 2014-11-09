//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//


#pragma once

#include <vector>

class Halfedge;
class Vertex;
class HMesh;

class Face {
public:
    Face(HMesh* hMesh):hMesh{hMesh}{}
    Face(const Face&) = delete;
    ~Face();
    int edgeCount();
    std::vector<Halfedge*> circulate();

    void reassignFaceToEdgeLoop();

    Vertex* split();

    bool isValid();

    Halfedge* halfedge = nullptr;
private:
    HMesh* hMesh;

    friend class HMesh;
    friend class HMesh;
};



