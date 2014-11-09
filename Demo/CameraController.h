//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//


#pragma once
#include "kick/kick.h"


class CameraController : public kick::Component, public kick::Updatable {
    float mouseWheelSpeed = 0.01f;
    float mouseMovementSpeed = 0.01f;
    float mouseRotationSpeed = 0.005f;

    glm::vec3 euler = glm::vec3(0);
    kick::Transform *target = nullptr;
    kick::Transform *transform= nullptr;

    bool zoomToMouse = true;
    kick::CameraPerspective *perspective;
public:

    CameraController(kick::GameObject *gameObject);

    virtual void update();

    void dolly();

    void pan();

    void rotate();

    void orbit();

    void setTarget(kick::Transform *target);
private:
    glm::vec3 rotateAroundPoint(glm::vec3 input, glm::vec3 pivotPoint);
};



