//
//  main.cpp
//  SimpleHMesh
//
//  Created by Morten Nobel-Jørgensen on 16/10/14.
//  Copyright (c) 2014 Morten Nobel-Jørgensen. All rights reserved.
//

#include "kick/kick.h"
#include "HMeshComponent.h"
#include "CameraController.h"

using namespace kick;

int main(int argc, char * argv[])
{
    Engine::init(argc, argv);
    auto scene = Engine::activeScene();
    auto camera = scene->createPerspectiveCamera();
    camera->gameObject()->transform()->setLocalPosition({0,0,10});
    CameraController* cameraController = camera->gameObject()->addComponent<CameraController>();
    MeshRenderer *mr = scene->createPlane();
    cameraController->setTarget(mr->transform());
    mr->setMaterial( new Material(Project::loadShader("shaders/line.shader")));
    auto mMeshComp = mr->gameObject()->addComponent<HMeshComponent>();

    scene->createDirectionalLight();
    Engine::startMainLoop();

    return 0;
}