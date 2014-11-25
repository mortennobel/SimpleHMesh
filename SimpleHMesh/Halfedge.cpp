//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include "Halfedge.h"
#include "Vertex.h"
#include "Face.h"
#include "HMesh.h"
#include <iostream>

using namespace std;

Halfedge::Halfedge(HMesh* hMesh) : hMesh{hMesh} {
#ifdef DEBUG
    static int idGlobal = 0;
    id = idGlobal++;
#endif
}

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
    assert(vert->halfedge->hMesh->isValid());
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
    assert(hMesh->isValid());
}

Vertex* Halfedge::split(){
    Vertex* vertex = hMesh->createVertex();
    vertex->position = (vert->position + prev->vert->position)*0.5f;
    Halfedge* newHE = splitInternal(vertex);
    if (opp != nullptr){
        Halfedge* newOppHE = opp->splitInternal(vertex);

        newHE->glue(opp);
        this->glue(newOppHE);
    }
    assert(hMesh->isValid());

    return vertex;
}


Halfedge* Halfedge::splitInternal(Vertex* newVertex){
    Halfedge* oldPrev = prev;

    Halfedge* newPrev = hMesh->createHalfedge();

    // Link halfedges
    oldPrev->link(newPrev);
    newPrev->link(this);

    // Link vertices
    newPrev->link(newVertex);
    oldPrev->link(oldPrev->vert);

    newPrev->link(face);


    return newPrev;
}

bool Halfedge::isValidEdgeLoop(){
    int count;
    Halfedge *he = this;

    while (he!=this || count == 0){
        he = he->next;
        count++;
        if (he->face != face){
            return false;
        }
        if (!he){
            return false;
        }
        if (count == 10000){
            return false;
        }
    }

    count = 0;
    while (he!=this || count == 0 ){
        he = he->prev;

        count++;
        if (he->face != face){
            return false;
        }
        if (!he){
            return false;
        }
        if (count == 10000){
            return false;
        }
    }
    return true;
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
    assert(next != nullptr);
    vertex->halfedge = next;
    vert = vertex;
}

bool Halfedge::isBoundary() {
    return opp == nullptr;
}

void Halfedge::glue(Halfedge* oppEdge) {
    if (oppEdge == this){
        assert(false);//"Glue to self";
    }
    opp = oppEdge;
    oppEdge->opp = this;
}

bool Halfedge::isValid() const {
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
    if (valid){
        valid = hMesh->existsOrNull(this) &&
                hMesh->existsOrNull(this->vert) &&
                hMesh->existsOrNull(this->next) &&
                hMesh->existsOrNull(this->prev) &&
                hMesh->existsOrNull(this->opp) &&
                hMesh->existsOrNull(this->face)
                ;
    }

    return valid;
}

float Halfedge::length() {
    glm::vec3 vertexPos = vert->position;
    glm::vec3 lastVertexPos = prev->vert->position;
    return glm::length(vertexPos - lastVertexPos);
}

std::ostream &operator<<(std::ostream& os, Halfedge *dt) {
#ifdef DEBUG
    os << "Halfedge{ id: "<< dt->id<<",next:"<< (dt->next?dt->next->id:-1)<<",prev:"<< (dt->prev?dt->prev->id:-1)
            <<",opp:"<< (dt->opp?dt->opp->id:-1)<<",vert:"<< (dt->vert?dt->vert->id:-1)<<",face:"<< (dt->face?dt->face->id:-1)<<"}";
#else
    os << "Halfedge{ id: "<< (void*)dt<<",next:"<< (void*)dt->next<<",prev:"<< (void*)dt->prev
            <<",opp:"<< (void*)dt->opp<<",vert:"<< (void*)dt->vert<<",face:"<< (void*)dt->face<<"}";
#endif
    return os;
}

void Halfedge::dissolve() {
    assert(this->opp);
    prev->link(opp->next);
    opp->prev->link(next);

    prev->link(face);
    face->reassignFaceToEdgeLoop();
    auto faceToDelete = opp->face;
    opp->face = nullptr;
    hMesh->destroy(faceToDelete);
    auto oppToDelete = opp;
    opp = nullptr;
    hMesh->destroy(oppToDelete);
    hMesh->destroy(this);


    prev->isValid();
    face->isValid();
    assert(prev->hMesh->isValid());

}

glm::vec3 Halfedge::direction() {
    return vert->position - prev->vert->position;
}
