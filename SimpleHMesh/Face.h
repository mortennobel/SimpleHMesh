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
    std::vector<Halfedge*> circulate();

    void reassignFaceToEdgeLoop();

    Vertex* split();

    bool isValid();

    Halfedge* halfedge = nullptr;

    Face* connect(Vertex *pVertex, Vertex *pVertex1);
#ifdef DEBUG
    int id;
#endif
private:
    HMesh* hMesh;

    friend class HMesh;
    friend class HMesh;

};

std::ostream & operator<<(std::ostream& os, Face *dt);



