//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <assert.h>
#include "Vertex.h"
#include "Halfedge.h"
#include "HMesh.h"

using namespace std;

Vertex::Vertex(HMesh* hMesh) : hMesh{hMesh} {
#ifdef DEBUG
    static int idGlobal = 0;
    id = idGlobal++;
#endif
}

Vertex::~Vertex() {
    if (hMesh){
        hMesh = nullptr;
    }
}

std::vector<Halfedge*> Vertex::circulate(){
    std::vector<Halfedge*> res;

    Halfedge* iter = halfedge;
    bool first = true;
    while (iter != nullptr && (iter != halfedge || first)){
        res.push_back(iter);
        first = false;
        // goto next out-going halfedge
        iter = iter->opp;
        if (iter != nullptr){
            iter = iter->next;
        }
    }

    bool meetBoundary = iter == nullptr;
    if (meetBoundary){
        // circulate other way
        iter = halfedge->prev->opp;
        while (iter != nullptr){
            res.insert(res.begin(),iter);
            iter = iter->prev;
            if (iter != nullptr){
                iter = iter->opp;
            }
        }
    }
    return res;
}

std::vector<Halfedge*> Vertex::circulateOpp() {
    vector<Halfedge*> res;
    Halfedge* iter = halfedge->prev;
    bool first = true;
    while (iter != nullptr && (iter != halfedge || first)){
        res.push_back(iter);
        first = false;
        // goto next out-going halfedge
        iter = iter->opp;
        if (iter != nullptr){
            iter = iter->prev;
        }
    }

    bool meetBoundary = iter == nullptr;
    if (meetBoundary){
        // circulate other way
        iter = halfedge->opp;
        while (iter != nullptr){
            res.insert(res.begin(),iter);
            iter = iter->next;
            if (iter != nullptr){
                iter = iter->opp;
            }
        }
    }
    return res;
}

std::vector<Halfedge*> Vertex::circulateOneRing(){
    std::vector<Halfedge*> res;
    if (isBoundary()){
        assert(false); // "Cannot iterate one ring of vertex when boundary vertex");
        return res;
    }
    Halfedge* iter = halfedge->next;
    Halfedge* firstHE = iter;
    bool first = true;
    while (iter != firstHE || first){
        res.push_back(iter);
        iter = iter->next->opp->next;
        first = false;
    }
    return res;
}

bool Vertex::isValid() const{
    bool valid = true;
    if (halfedge == nullptr){
        assert(false);//"Halfedge is null");
        valid = false;
    }
    if (halfedge->prev->vert != this){
        assert(false);//"Vertex is not correctly associated with halfedge.";
        valid = false;
    }
    if (valid){
        valid = hMesh->existsOrNull(this) &&
                hMesh->existsOrNull(this->halfedge)
                ;
    }

    return valid;
}

void Vertex::replaceVertex(Vertex* newVertex) {
    for (auto & he : circulateOpp()){
        he->link(newVertex);
    }
}

bool Vertex::isBoundary() {
    Halfedge* iter = halfedge;
    bool first = true;
    while (iter != nullptr && (iter != halfedge || first)){
        first = false;
        // goto next out-going halfedge
        iter = iter->opp;
        if (iter != nullptr){
            iter = iter->next;
        }
    }
    return iter == nullptr;
}

std::ostream &operator<<(std::ostream& os, Vertex *dt) {
#ifdef DEBUG
    os << "Vertex{ id: "<< dt->id << ",halfedge:" << (dt->halfedge?dt->halfedge->id:-1) << ",position:" << glm::to_string(dt->position) << "}";
#else
    os << "Vertex{ id: "<< (void*)dt << ",halfedge:" << (void*)dt->halfedge << ",position:" << glm::to_string(dt->position) << "}";
#endif
    return os;
}

void Vertex::dissolve() {
    assert(hMesh);
    assert(circulate().size()<=2);
    assert(halfedge);

    bool boundary = isBoundary();

    // link halfedges
    halfedge->prev->link(halfedge->next);
    if (!boundary){
        halfedge->opp->prev->link(halfedge->opp->next);
    }

    // link vertices
    halfedge->prev->link(halfedge->vert);

    // link faces (make sure no dangling pointer);
    halfedge->face->halfedge = halfedge->prev;
    if (!boundary){
        halfedge->opp->face->halfedge = halfedge->opp->prev;
    }

    HMesh* mMeshRef = hMesh;


    if (!boundary){
        hMesh->destroy(halfedge->opp);
    }
    hMesh->destroy(halfedge);
    hMesh->destroy(this);
    assert(mMeshRef->isValid());

}
