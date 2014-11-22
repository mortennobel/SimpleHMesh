//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include "HMesh.h"

#include <vector>
#include <set>
#include <tuple>
#include <iostream>
#include <unordered_map>
#include "Vertex.h"
#include "Halfedge.h"
#include "Face.h"


using namespace std;
using namespace boost::container;

namespace {
    template<typename T>
    bool eraseElement(boost::container::stable_vector<T> & vector, T* ptr){
        for (auto iter = vector.begin();iter != vector.end();iter++){
            T* iterPtr = &(*iter);
            if (iterPtr == ptr){
                vector.erase(iter);
                return true;
            }
        }
        return false;
    }
}

Vertex *HMesh::createVertex() {
    mVertex.emplace_back(this);
    return &mVertex.back();
}

Halfedge *HMesh::createHalfedge() {
    mHalfedge.emplace_back(this);
    return &mHalfedge.back();
}

Face *HMesh::createFace() {
    mFace.emplace_back(this);
    return &mFace.back();
}

void HMesh::destroy(Vertex *v) {
    eraseElement<Vertex>(mVertex, v);
    v->hMesh = nullptr;
}

void HMesh::destroy(Halfedge *he) {
    eraseElement<Halfedge>(mHalfedge, he);
    he->hMesh = nullptr;
}

void HMesh::destroy(Face *f) {
    eraseElement<Face>(mFace, f);
    f->hMesh = nullptr;
}

void HMesh::clear() {
    mVertex.clear();
    mHalfedge.clear();
    mFace.clear();
}

namespace{
    struct TupleHash : public unary_function<tuple<int,int>, size_t>{
        size_t operator()(tuple<int,int> x) const _NOEXCEPT {
            return std::get<0>(x) ^ std::get<1>(x);
        }
    };

}

void HMesh::build(const std::vector<glm::vec3>& vertices, const std::vector<int>& indices){
    clear();

    std::vector<Vertex*> vertexList;

    unordered_map<tuple<int,int>, Halfedge*, TupleHash> halfedgeByVertexID;

    for (size_t i=0;i<vertices.size();i++){
        auto newV = createVertex();
        newV->position = vertices[i];
        vertexList.push_back(newV);

    }
    for (int i=0;i<indices.size();i+=3){
        int idx[3]{
                indices[i],
                indices[i+1],
                indices[i+2]};
        Halfedge* edges[3];
        Face* face = createFace();
        for (int j=0;j<3;j++){
            Halfedge* edge = createHalfedge();
            edge->link(face);
            edges[j] = edge;
        }
        for (int j=0;j<3;j++){
            int from = idx[j];
            int to = idx[(j+1)%3];
            edges[j]->link(edges[(j+1)%3]);
            edges[j]->link(vertexList[to]);
            halfedgeByVertexID[tuple<int,int>{from, to}] = edges[j];
        }
    }

    // glue all opposite half edges
    for (auto keyValue : halfedgeByVertexID){
        if (std::get<0>(keyValue.first) < std::get<1>(keyValue.first)){
            continue; // skip half of the halfedges (this avoids unneeded glue)
        }
        tuple<int,int> otherKey{std::get<1>(keyValue.first), std::get<0>(keyValue.first) };

        auto iter = halfedgeByVertexID.find(otherKey);
        if (iter != halfedgeByVertexID.end()){
            keyValue.second->glue(iter->second);
        }
    }
    isValid();
}

// polygons
void HMesh::build(const std::vector<glm::vec3>& vertices, const std::vector<std::vector<int>> & indicesPoly){
    clear();

    std::vector<Vertex*> vertexList;

    unordered_map<tuple<int,int>, Halfedge*, TupleHash> halfedgeByVertexID;

    for (size_t i=0;i<vertices.size();i++){
        auto newV = createVertex();
        newV->position = vertices[i];
        vertexList.push_back(newV);

    }
    for (auto& poly : indicesPoly){
        std::vector<Halfedge*> edges(poly.size());
        Face* face = createFace();
        for (int j=0;j<poly.size();j++){
            Halfedge* edge = createHalfedge();
            edge->link(face);
            edges[j] = edge;
        }
        for (int j=0;j<poly.size();j++){
            int from = poly[j];
            int to = poly[(j+1)%poly.size()];
            edges[j]->link(edges[(j+1)%poly.size()]);
            edges[j]->link(vertexList[to]);
            halfedgeByVertexID[tuple<int,int>{from, to}] = edges[j];
        }
    }

    // glue all opposite half edges
    for (auto keyValue : halfedgeByVertexID){
        if (std::get<0>(keyValue.first) < std::get<1>(keyValue.first)){
            continue; // skip half of the halfedges (this avoids unneeded glue)
        }
        tuple<int,int> otherKey{std::get<1>(keyValue.first), std::get<0>(keyValue.first) };

        auto iter = halfedgeByVertexID.find(otherKey);
        if (iter != halfedgeByVertexID.end()){
            keyValue.second->glue(iter->second);
        }
    }

    isValid();
}

void HMesh::exportMesh(std::vector<glm::vec3> outVertices, std::vector<int> outIndices){

}

const std::vector<Vertex*> HMesh::vertices() {
    std::vector<Vertex*> res;
    for (auto & v : mVertex){
        res.push_back(&v);
    }
    return res;
}

const std::vector<Halfedge*> HMesh::halfedges() {
    std::vector<Halfedge*> res;
    for (auto & he : mHalfedge){
        res.push_back(&he);
    }
    return res;
}

const std::vector<Face*> HMesh::faces() {
    std::vector<Face*> res;
    for (auto & f : mFace){
        res.push_back(&f);
    }
    return res;
}

void HMesh::printDebug() {
    cout << "Faces:" << endl;
    for (auto & f : mFace){
        cout << " "<<&f<< endl;
    }
    cout << "HE:" << endl;
    for (auto & he : mHalfedge){
        cout << " "<<&he<< endl;
    }
    cout << "Vertices:" << endl;
    for (auto & v : mVertex){
        cout << " "<<&v<< endl;
    }
}

const std::vector<Halfedge *> HMesh::halfedgesSingleSide() {
    std::vector<Halfedge*> res;
    std::set<Halfedge*> oppUsed;
    for (Halfedge & he : mHalfedge){
        if (he.opp == nullptr || oppUsed.find(he.opp) == oppUsed.end()){
            res.push_back(&he);
            oppUsed.insert(&he);
        }
    }
    return res;
}

int HMesh::faceCount() {
    return mFace.size();
}

int HMesh::vertexCount() {
    return mVertex.size();
}

int HMesh::halfedgeCount() {
    return mHalfedge.size();
}

Vertex *HMesh::vertex(int id) {
    return &(mVertex[id]);
}

Halfedge *HMesh::halfedge(int id) {
    return &(mHalfedge[id]);
}

Face *HMesh::face(int id) {
    return &(mFace[id]);
}

bool HMesh::isValid() {
    bool valid = true;
    for (const auto& f : mFace){
        valid &= f.isValid();
        if (isUnused(&f)){
            cout << "Face "<<f.id<<" unused"<<endl;
        }
    }
    for (const auto& f : mHalfedge){
        valid &= f.isValid();
        if (isUnused(&f)){
            cout << "Halfedge "<<f.id<<" unused"<<endl;
        }
    }
    for (const auto& f : mVertex){
        valid &= f.isValid();
        if (isUnused(&f)){
            cout << "Vertex "<<f.id<<" unused"<<endl;
        }
    }
    return valid;
}

bool HMesh::isUnused(const Vertex* v){
    for (const auto& f : mHalfedge){
        if (f.vert == v){
            return false;
        }
    }

    return true;
}
bool HMesh::isUnused(const Halfedge* he){
    for (const auto& f : mFace){
        if (f.halfedge == he){
            return false;
        }
    }
    for (const auto& f : mHalfedge){
        if (f.next == he){
            return false;
        }
        if (f.prev == he){
            return false;
        }
        if (f.opp == he){
            return false;
        }
    }
    for (const auto& f : mVertex){
        if (f.halfedge == he){
            return false;
        }
    }
    return true;
}
bool HMesh::isUnused(const Face* f){
    for (const auto& he : mHalfedge){
        if (he.face == f){
            return false;
        }
    }
    return true;
}

bool HMesh::existsOrNull(const Vertex *v) {
    if (v== nullptr){
        return true;
    }
    for (const auto& f : mVertex){
        if (&f == v){
            return true;
        }
    }
    cout << "Error finding v "<<v->id<<endl;
    return false;
}

bool HMesh::existsOrNull(const Halfedge *h) {
    if (h == nullptr){
        return true;
    }
    for (const auto& he : mHalfedge){
        if (&he == h){
            return true;
        }
    }
    cout << "Error finding he "<<h->id<<endl;
    return false;
}

bool HMesh::existsOrNull(const Face *f) {
    if (f == nullptr){
        return true;
    }
    for (const auto& fIter : mFace){
        if (&fIter == f){
            return true;
        }
    }
    cout << "Error finding face "<<f->id<<endl;
    return false;
}
