//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include "HMesh.h"

#include <vector>
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
            if (&(*iter) == ptr){
                vector.erase(iter);
                return true;
            }
        }
        return false;
    }
}

Vertex *HMesh::createVertex() {
    vertex.emplace_back(this);
    return &vertex.back();
}

Halfedge *HMesh::createHalfedge() {
    halfedge.emplace_back(this);
    return &halfedge.back();
}

Face *HMesh::createFace() {
    face.emplace_back(this);
    return &face.back();
}

void HMesh::destroy(Vertex *v) {
    eraseElement<Vertex>(vertex, v);
}

void HMesh::destroy(Halfedge *he) {
    eraseElement<Halfedge>(halfedge, he);
}

void HMesh::destroy(Face *f) {
    eraseElement<Face>(face, f);
}

void HMesh::clear() {
    vertex.clear();
    halfedge.clear();
    face.clear();
}

namespace{
    struct TupleHash : public unary_function<tuple<int,int>, size_t>{
        size_t operator()(tuple<int,int> x) const _NOEXCEPT {
            return std::get<0>(x) ^ std::get<1>(x);
        }
    };

}

void HMesh::build(std::vector<glm::vec3> vertices, std::vector<int> indices){
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
}

void HMesh::exportMesh(std::vector<glm::vec3> outVertices, std::vector<int> outIndices){

}

const std::vector<Vertex*> HMesh::vertices() {
    std::vector<Vertex*> res;
    for (auto & v : vertex){
        res.push_back(&v);
    }
    return res;
}

const std::vector<Halfedge*> HMesh::halfedges() {
    std::vector<Halfedge*> res;
    for (auto & he : halfedge){
        res.push_back(&he);
    }
    return res;
}

const std::vector<Face*> HMesh::faces() {
    std::vector<Face*> res;
    for (auto & f : face){
        res.push_back(&f);
    }
    return res;
}

void HMesh::printDebug() {
    cout << "Faces:" << endl;
    for (auto & f : face){
        cout << " "<<&f<< endl;
    }
    cout << "HE:" << endl;
    for (auto & he : halfedge){
        cout << " "<<&he<< endl;
    }
    cout << "Vertices:" << endl;
    for (auto & v : vertex){
        cout << " "<<&v<< endl;
    }
}
