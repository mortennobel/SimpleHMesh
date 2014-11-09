//
// Created by Morten Nobel-Jørgensen on 07/11/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include "HMeshConverter.h"
#include "Vertex.h"
#include "Face.h"
#include "Halfedge.h"

#include <vector>
#include <functional>
#include <unordered_map>

using namespace kick;
using namespace std;

namespace std
{
    template<>
    struct hash<tuple<int,int>>
    {
        typedef tuple<int,int> argument_type;
        typedef std::size_t result_type;

        result_type operator()(argument_type const& s) const
        {
            result_type const h1 ( std::hash<int>()(std::get<0>(s)) );
            result_type const h2 ( std::hash<int>()(std::get<1>(s)) );
            return h1 ^ (h2 << 1);
        }
    };
}

void HMeshConverter::convert(HMesh *source, kick::MeshData *dest) {
    auto vertexList = source->vertices();

    vector<glm::vec3> vertexArray(vertexList.size());
    for (int i=0;i<vertexList.size();i++){
        vertexArray[i] = vertexList[i]->position;
    }
    dest->setPosition(vertexArray);
    dest->recomputeNormals();

    vector<unsigned short> triangles;
    for (Face* face : source->faces()){
        if (face->edgeCount() != 3){
            logWarning("Only triangles supported");
            continue;
        }
        Halfedge *he = face->halfedge;
        bool first = true;
        while (he != face->halfedge || first){

            int indexOfVertex = 0;
            auto iter = vertexList.begin();
            while (*iter != he->vert) {
                indexOfVertex++;
                iter++;
            }
            triangles.push_back(indexOfVertex);
            he = he->next;
            first = false;
        }
    }
    dest->setSubmesh(0, triangles, MeshType::Triangles);

}

void HMeshConverter::convert(kick::MeshData *source, HMesh *dest) {
    if (source->submeshesCount() != 1){
        logError("Only support a single submesh");
        return;
    }
    if (source->submeshType(0) != MeshType::Triangles){
        logError("Only supports triangles");
        return;
    }
    vector<Vertex*> vertexList;
    unordered_map<tuple<int,int>, Halfedge*> halfedgeByVertexID;
    for (int i=0;i<source->position().size();i++){
        auto newV = dest->createVertex();
        newV->position = source->position()[i];
        vertexList.push_back(newV);
    }
    for (int i=0;i<source->submeshSize(0);i+=3){
        unsigned short idx[] = {
                source->submeshIndices(0)[i+0],
                source->submeshIndices(0)[i+1],
                source->submeshIndices(0)[i+2],
        };
        Halfedge *edges[3];
        Face *face = dest->createFace();
        for (int j=0;j<3;j++){
            edges[j] = dest->createHalfedge();
            edges[j]->link(face);
        }
        for (int j=0;j<3;j++){
            int from = idx[j];
            int to = idx[(j+1)%3];
            edges[j]->link(edges[(j+1)%3]);
            edges[j]->link(vertexList[to]);
            halfedgeByVertexID.emplace(tuple<int,int>{from, to}, edges[j]);
        }
    }
    // glue all opposite half edges
    for (auto keyValue : halfedgeByVertexID){
        int first = get<0>(keyValue.first);
        int second = get<1>(keyValue.first);
        if (first < second){
            continue;  // skip half of the halfedges (this avoids unneeded glue)
        }
        tuple<int,int> otherKey{second, first};
        auto other = halfedgeByVertexID.find(otherKey);
        if (other != halfedgeByVertexID.end()){
            keyValue.second->glue(other->second);
        }
    }

}
