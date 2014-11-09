//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include <iostream>
#include "Face.h"
#include "Halfedge.h"
#include "Vertex.h"
#include "HMesh.h"

using namespace std;

Face::~Face() {
    if (hMesh){
        hMesh = nullptr;
    }
}

int Face::edgeCount() {
    int count = 1;
    Halfedge* current = halfedge->next;
    while (current != halfedge){
        count++;
        current = current->next;
    }
    return count;
}

std::vector<Halfedge *> Face::circulate() {
    std::vector<Halfedge *> res;
    Halfedge *iter = halfedge;
    bool first = true;
    while (iter != halfedge || first){
        res.push_back(iter);
        first = false;
        iter = iter->next;
    }
    return res;
}

void Face::reassignFaceToEdgeLoop() {
    for (auto he : circulate()){
        he->link(this);
    }
}

Vertex *Face::split() {
    Vertex* v = hMesh->createVertex();
    vector<Halfedge*> newHalfedges;
    for (auto he : circulate()){
        v->position += he->vert->position;

        Halfedge* toNewVertex = hMesh->createHalfedge();
        Halfedge* fromNewVertex = hMesh->createHalfedge();
        toNewVertex->glue(fromNewVertex);
        newHalfedges.push_back(toNewVertex);
    }
    int count = 0;
    bool first = true;

    // second iteration - link everything together
    for (auto he : circulate()){
        int prevIdx = (count-1+newHalfedges.size())%newHalfedges.size();
        Halfedge *prevNewEdge = newHalfedges[prevIdx];
        Halfedge *newEdge = newHalfedges[count];

        // link halfedges
        newEdge->opp->link(he->next);
        he->link(newEdge);
        newEdge->link(prevNewEdge);

        // link vertices
        newEdge->opp->link(he->vert);
        newEdge->link(v);

        if (!first){
            Face* newFace = hMesh->createFace();
            newFace->halfedge = he;
            newFace->reassignFaceToEdgeLoop();
        }

        Halfedge *toNewVertex = hMesh->createHalfedge();
        Halfedge *fromNewVertex = hMesh->createHalfedge();
        toNewVertex->glue(fromNewVertex);

        first = false;
        count++;
    }
    v->position = v->position * (1.0f/count);

    return v;
}

bool Face::isValid() {
    bool valid = true;
    if (halfedge == nullptr){
        cerr << "Halfedge is null"<<endl;
        valid = false;
    }
    for (auto he : circulate()){
        if (he->face != this){
            cerr << "Halfedge.face is not uniform along halfedges" << endl;
            valid = false;
        }
    }
    return valid;
}

