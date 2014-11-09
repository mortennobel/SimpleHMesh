//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include "Halfedge.h"
#include "Vertex.h"
#include "Face.h"
#include "HMesh.h"


Halfedge::~Halfedge() {
    if (hMesh){
        hMesh = nullptr;
    }
}

Vertex* Halfedge::collapse(bool center){
    if (center){
        vert->position = (vert->position + prev->vert->position)*0.5f;
    }
    collapseInternal(false);
    if (opp != nullptr){
        opp->collapseInternal(true);
        hMesh->destroy(opp);
    }
    hMesh->destroy(this);

    return vert;
}

void Halfedge::collapseInternal(bool opp){

    if (!opp) {
        prev->vert->replaceVertex(vert);
        hMesh->destroy(prev->vert);
    } else {
        vert->replaceVertex(prev->vert);
        hMesh->destroy(vert);
    }
    prev->opp->glue(next->opp);
    for (auto he : face->circulate()){
        hMesh->destroy(he);
    }
    hMesh->destroy(face);
}

Halfedge* Halfedge::splitInternal(Vertex* vertex){
    if (face->edgeCount() != 3){
        assert(false);// ("Can only Split edge between two triangles");
    }
    Halfedge* oldNext = next;
    Halfedge* oldPrev = prev;

    Vertex* oppositeVert = next->vert;

    Halfedge* newPrev = hMesh->createHalfedge();
    Halfedge* splitEdge1 = hMesh->createHalfedge();
    Halfedge* splitEdge2 = hMesh->createHalfedge();
    splitEdge1->glue(splitEdge2);

    // Link halfedges
    oldPrev->link(newPrev);
    newPrev->link(splitEdge1);
    splitEdge1->link(oldPrev);

    oldNext->link(splitEdge2);
    splitEdge2->link(this);

    // Link vertices
    splitEdge1->link (oppositeVert);
    splitEdge2->link (vertex);
    newPrev->link(vertex);
    oldPrev->link(oldPrev->vert); // set correct vertex link

    link(face);
    face->reassignFaceToEdgeLoop();
    Face* newFace = hMesh->createFace();
    newPrev->link(newFace);
    newFace->reassignFaceToEdgeLoop();

    face->isValid();
    newFace->isValid();
    oppositeVert->isValid();
    vertex->isValid();
    splitEdge1->isValid();
    splitEdge2->isValid();

    return newPrev;
}

void Halfedge::flip() {
    if (isBoundary()){
        assert(false);//("Cannot flip boundary edge");
        return;
    }
    if (face->edgeCount() != 3 || opp->face->edgeCount() != 3){
        assert(false); // ("Can only flip edge between two triangles");
    }
    Halfedge* oldNext = next;
    Halfedge* oldPrev = prev;
    Halfedge* oldOppNext = opp->next;
    Halfedge* oldOppPrev = opp->prev;

    Vertex* thisVert = vert;
    Vertex* oppVert = opp->vert;
    Vertex* thisOppositeVert = next->vert;
    Vertex* oppOppositeVert = opp->next->vert;

    // flip halfedges
    this->link(oldPrev);
    oldNext->link(opp);
    opp->link(oldOppPrev);
    oldOppNext->link(this);

    oldOppPrev->link(oldNext);
    oldPrev->link (oldOppNext);

    // reassign vertices
    this->link(thisOppositeVert);
    opp->link(oppOppositeVert);

    face->reassignFaceToEdgeLoop();
    opp->face->reassignFaceToEdgeLoop();
}

Vertex* Halfedge::split(){
    Vertex* vertex = hMesh->createVertex();
    vertex->position = (vert->position+prev->vert->position)*0.5f;
    Halfedge* newHE = splitInternal(vertex);
    if (opp != nullptr){
        Halfedge* newOppHE = opp->splitInternal(vertex);
        newHE->glue(opp);
        this->glue(newOppHE);
        newOppHE->isValid();
    }
    newHE->isValid();
    return vertex;
}

void Halfedge::link(Halfedge* nextEdge) {
    if (this == nextEdge){
        assert(false); // Link of self
    }
    next = nextEdge;
    nextEdge->prev = this;
}

void Halfedge::link(Face* face){
    this->face = face;
    face->halfedge = this;
}

void Halfedge::link(Vertex* vertex){
    if (next == nullptr){
        assert(false);//("next pointer is null");
    }
    vertex->halfedge = next;
    vert = vertex;
}

bool Halfedge::isBoundary() {
    return opp== nullptr;
}

void Halfedge::glue(Halfedge* oppEdge) {
    if (oppEdge == this){
        assert(false);//"Glue to self";
    }
    opp = oppEdge;
    oppEdge->opp = this;
}

bool Halfedge::isValid() {
    bool valid = true;
    if (opp != nullptr && opp->opp != this){
        assert(false);// ("opp is different from this or null");
        valid = false;
    }
    if (prev->next != this){
        assert(false);// ("prev.next is different from this");
        valid = false;
    }
    if (next->prev != this){
        assert(false);// ("next.prev is different from this");
        valid = false;
    }

    return valid;
}
