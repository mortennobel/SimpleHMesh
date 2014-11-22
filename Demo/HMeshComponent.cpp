//
// Created by Morten Nobel-Jørgensen on 08/11/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include "HMeshComponent.h"
#include "HMeshConverter.h"
#include <iostream>
#include <set>
#include "Face.h";
#include "Vertex.h";
#include "Halfedge.h";

using namespace kick;
using namespace std;

void HMeshComponent::update() {
    if (KeyInput::anyDown()){
        HMesh hmesh{};
        MeshData *meshData = mesh->meshData();
        HMeshConverter::convert(meshData, &hmesh);

        cout << "pre"<<endl;
        hmesh.printDebug();

        int nVert = hmesh.vertices().size();
        int nFaces = hmesh.faces().size();
        int nHalfedges = hmesh.halfedges().size();
        if (KeyInput::down(Key::NUM_1) || KeyInput::down(Key::KP_1)){
            cout << "FaceSplit" << endl;
            for (auto f : hmesh.faces()) {
                f->split();
            }
        } else if (KeyInput::down(Key::NUM_2) || KeyInput::down(Key::KP_2)) {
            cout << "EdgeSplit" << endl;
            set<Halfedge*> isSplit;
            for (auto h : hmesh.halfedges()){
                if (!h->isBoundary() && isSplit.find(h) == isSplit.end()){
                    isSplit.insert(h->opp);
                    Vertex* thisVertex = h->next->vert;
                    Vertex* oppVertex = h->opp->next->vert;
                    Vertex* newVertex = h->split();
                    h->face->connect(newVertex, thisVertex);
                    h->opp->face->connect(newVertex, oppVertex);
                }
            }
        } else if (KeyInput::down(Key::NUM_3) || KeyInput::down(Key::KP_3)) {
            cout << "CollapseEdge" << endl;
            set<Halfedge*> isCollapsed;
            for (auto h : hmesh.halfedges()){
                if (!h->isBoundary()){
                    if (!h->vert->isBoundary() && !h->prev->vert->isBoundary() && isCollapsed.find(h) != isCollapsed.end()){
                        isCollapsed.insert(h->opp);
                        h->collapse();
                        break; // only break first edge
                    }
                }
            }
        } else {
            cout << "Convert only" << endl;
        }
        int postVert = hmesh.vertices().size();
        int postFaces = hmesh.faces().size();
        int postHalfedges = hmesh.halfedges().size();
        printf("vert %i to %i faces %i to %i he %i to %i\n", nVert,postVert, nFaces, postFaces, nHalfedges, postHalfedges);
        HMeshConverter::convert(&hmesh, meshData);

        mesh->setMeshData(meshData);
    }
}