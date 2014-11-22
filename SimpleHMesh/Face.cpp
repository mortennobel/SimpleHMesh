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

Face::Face(HMesh* hMesh) : hMesh{hMesh} {
#ifdef DEBUG
    static int idGlobal = 0;
    id = idGlobal++;
#endif

}

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

std::vector<Halfedge *> Face::circulate()  const {
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
    vector<Vertex*> vertices;
    for (auto he : circulate()){
        v->position += he->vert->position;
        Halfedge* toNewVertex = hMesh->createHalfedge();
        Halfedge* fromNewVertex = hMesh->createHalfedge();
        toNewVertex->glue(fromNewVertex);
        newHalfedges.push_back(toNewVertex);

        vertices.push_back(he->vert);
    }
    int count = 0;
    bool first = true;

    // second iteration - link everything together
    for (auto he : circulate()){
        int prevIdx = (count-1+newHalfedges.size())%newHalfedges.size();
        Halfedge *prevNewEdge = newHalfedges[prevIdx]->opp;
        Halfedge *newEdge = newHalfedges[count];

        // link halfedges
        prevNewEdge->link(he);
        he->link(newEdge);
        newEdge->link(prevNewEdge);

        // link vertices
        newEdge->link(v);
        prevNewEdge->link(vertices[prevIdx]);

        if (!first) {
            Face* newFace = hMesh->createFace();
            newFace->halfedge = he;
            newFace->reassignFaceToEdgeLoop();
        } else {
            halfedge = he;
            reassignFaceToEdgeLoop();
        }

        first = false;
        count++;
    }

    v->position = v->position * (1.0f/count);

    assert(hMesh->isValid());

    return v;
}

bool Face::isValid()  const{
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
    if (valid){
        valid = hMesh->existsOrNull(this) && hMesh->existsOrNull(this->halfedge);
    }
    return valid;
}

Halfedge* Face::connect(Vertex *pVertex1, Vertex *pVertex2) {
    assert( edgeCount() > 3);
    Halfedge* he1 = nullptr;
    Halfedge* he2 = nullptr;

    for (auto i : circulate()){
        if (i->vert == pVertex1){
            he1 = i;
        } else if (i->vert == pVertex2){
            he2 = i;
        }
    }
    assert(he1 && he2);
    Halfedge* he1Next = he1->next;
    Halfedge* he2Next = he2->next;

    Halfedge* splitEdge1 = hMesh->createHalfedge();
    Halfedge* splitEdge2 = hMesh->createHalfedge();
    splitEdge1->glue(splitEdge2);
    Face* newFace = hMesh->createFace();

    // link vertices
    splitEdge1->vert = pVertex1;
    splitEdge2->vert = pVertex2;

    // link edges
    splitEdge1->link(he1Next);
    he2->link(splitEdge1);
    splitEdge2->link(he2Next);
    he1->link(splitEdge2);


    newFace->halfedge = splitEdge1;
    newFace->reassignFaceToEdgeLoop();

    this->halfedge = splitEdge2;
    reassignFaceToEdgeLoop();

    assert(hMesh->isValid());

    return splitEdge1;
}

float Face::area(){
    float  area=0.0f ;
    // inspired by http://paulbourke.net/geometry/polygonmesh/source1.c
    for (auto  he : circulate()){
        auto lastHe = he->prev;
        glm::vec3 hePos = he->vert->position;
        glm::vec3 heLastPos = lastHe->vert->position;
        area += hePos.x * heLastPos.y;
        area -= hePos.y * heLastPos.x;
    }

    area /= 2;
    return(area < 0 ? -area : area);
}

std::ostream &operator<<(std::ostream& os, Face *dt) {
#ifdef DEBUG
    os << "Face{ id: "<< dt->id<<",he:"<< (dt->halfedge?dt->halfedge->id:-1) <<"}";
#else
    os << "Face{ id: "<< (void*)dt<<",he:"<< (void*)dt->halfedge<<"}";
#endif
    return os;
}