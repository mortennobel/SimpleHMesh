//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//


#pragma once

#include <vector>
#include <iostream>
#include "Vertex.h"

class Halfedge;
class Vertex;
class HMesh;

class Face {
public:
    Face(HMesh* hMesh);
    Face(const Face&) = delete;
    ~Face();
    int edgeCount();
    std::vector<Halfedge*> circulate() const;

    void reassignFaceToEdgeLoop();

    Vertex* split();

    bool isValid() const;

    float area();

    Halfedge* halfedge = nullptr;

    Halfedge* connect(Vertex *pVertex, Vertex *pVertex1);
#ifdef DEBUG
    int id;
#endif
private:
    HMesh* hMesh;

    friend class HMesh;
    friend class HMesh;

};

std::ostream & operator<<(std::ostream& os, Face *dt);



