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

    const std::vector<Vertex*>  vertices();
    const std::vector<Halfedge*> halfedges();
    const std::vector<Face*>  faces();

    void build(std::vector<glm::vec3> vertices, std::vector<int> indices);
    void exportMesh(std::vector<glm::vec3> outVertices, std::vector<int> outIndices);
private:
    boost::container::stable_vector<Vertex> vertex;
    boost::container::stable_vector<Halfedge> halfedge;
    boost::container::stable_vector<Face> face;
    void destroy(Vertex* v);
    void destroy(Halfedge* he);
    void destroy(Face* v);
    friend class Vertex;
    friend class Halfedge;
    friend class Face;
};



