//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//


#include <assert.h>
#include <boost/container/stable_vector.hpp>
#include "Vertex.h"
#include <iostream>

#pragma once

class Vertex;
class Face;
class HMesh;

class Halfedge {
public:
    Halfedge(HMesh* hMesh);
    Halfedge(const Halfedge&) = delete;
    ~Halfedge();

    Halfedge* next = nullptr;
    Halfedge* prev = nullptr;
    Halfedge* opp = nullptr;
    Vertex* vert = nullptr;
    Face* face = nullptr;

    Vertex* collapse(bool center = false);
    void flip();

    Vertex* split();

    void link(Halfedge* nextEdge);
    void link(Face* face);
    void link(Vertex* vertex);

    bool isBoundary();

    void glue(Halfedge* oppEdge);

    bool isValid() const;
    float length();
#ifdef DEBUG
    int id;
#endif

    void dissolve();

    bool isValidEdgeLoop();
private:


    HMesh* hMesh;

    void collapseInternal(bool opp);


    friend class HMesh;

    Halfedge *splitInternal(Vertex *vertex);
};


std::ostream & operator<<(std::ostream& os, Halfedge *dt);


