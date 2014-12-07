//
// Created by Morten Nobel-Jørgensen on 08/11/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//


#pragma once

#include "kick/kick.h"

class HMeshComponent : public kick::Component, public kick::Updatable {
    std::shared_ptr<kick::Mesh> mesh;
public:
    HMeshComponent(kick::GameObject *gameObject) : Component(gameObject) {
        auto meshRenderer = gameObject->component<kick::MeshRenderer>();
        mesh = meshRenderer->mesh();
    }
    kick::MeshData *meshData;
    virtual void update();
};



