//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <assert.h>
#include "Vertex.h"
#include "Halfedge.h"
#include "HMesh.h"

using namespace std;

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

bool Vertex::isValid(){
    bool valid = true;
    if (halfedge == nullptr){
        assert(false);//"Halfedge is null");
        valid = false;
    }
    if (halfedge->prev->vert != this){
        assert(false);//"Vertex is not correctly associated with halfedge.";
        valid = false;
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

