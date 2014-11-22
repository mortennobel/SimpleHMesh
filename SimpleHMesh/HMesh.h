//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//


#pragma once

#include <vector>
#include "boost/container/stable_vector.hpp"
#include "glm/glm.hpp"
#include "Vertex.h"
#include "Face.h"
#include "Halfedge.h"

class HMesh {
public:
    Vertex* createVertex();
    Halfedge* createHalfedge();
    Face* createFace();
    void clear();

    const std::vector<Vertex*> vertices();
    const std::vector<Halfedge*> halfedges();
    const std::vector<Halfedge*> halfedgesSingleSide();
    const std::vector<Face*>  faces();

    // triangles
    void build(const std::vector<glm::vec3>& vertices, const std::vector<int> & indices);
    // polygons
    void build(const  std::vector<glm::vec3>& vertices, const std::vector<std::vector<int> > & indices);
    void exportMesh(std::vector<glm::vec3> outVertices, std::vector<int> outIndices);

    void printDebug();

    int faceCount();
    int vertexCount();
    int halfedgeCount();

    Vertex* vertex(int id);
    Halfedge* halfedge(int id);
    Face* face(int id);
    bool isValid();
private:
    boost::container::stable_vector<Vertex> mVertex;
    boost::container::stable_vector<Halfedge> mHalfedge;
    boost::container::stable_vector<Face> mFace;
    void destroy(Vertex* v);
    void destroy(Halfedge* he);
    void destroy(Face* v);
    bool isUnused(const Vertex* v);
    bool isUnused(const Halfedge* v);
    bool isUnused(const Face* v);
    bool existsOrNull(const Vertex *v);
    bool existsOrNull(const Halfedge *v);
    bool existsOrNull(const Face *v);
    friend class Vertex;
    friend class Halfedge;
    friend class Face;
};



