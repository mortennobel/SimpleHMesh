//
// Created by Morten Nobel-Jørgensen on 16/10/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#include "CameraController.h"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

using namespace kick;
using namespace glm;
using namespace std;

CameraController::CameraController(kick::GameObject *gameObject)
        : Component(gameObject) {
    transform = Component::transform();
    perspective = gameObject->component<CameraPerspective>();
}

void CameraController::update() {
    ivec2 mouseWheelDelta = MouseInput::mouseWheelDelta();

    if (KeyInput::pressed(Key::LALT) || KeyInput::pressed(Key::RALT )) {
        if (mouseWheelDelta.y) {
            dolly();
        }
        if (MouseInput::pressed(1)) {
            pan();
        }
        else if (MouseInput::pressed(2)) {
            if (MouseInput::down(2)) {

            }  else {
                orbit();
            }
        }
    } else {
        if (mouseWheelDelta.y){
            dolly();
        }
        if (MouseInput::pressed(1)){
            pan();
        }
        else if (MouseInput::pressed(2)) {
            rotate();
        }
    }
}

void CameraController::rotate() {
    ivec2 mouseDelta = MouseInput::positionDelta();

    if (mouseDelta != ivec2(0)){
            euler.x += mouseRotationSpeed * (float)mouseDelta.x;
            euler.y += mouseRotationSpeed * (float)mouseDelta.y;
            euler.y = clamp(euler.y, -(float)M_PI + epsilon<float>(), (float)M_PI - epsilon<float>());

            auto rotation = toQuat( orientate3(euler) );
            transform->setRotation(rotation);
        }
}

void CameraController::pan() {
    ivec2 mouseDelta = MouseInput::positionDelta();
    if (mouseDelta != ivec2(0)){
            transform->setPosition(transform->position()
                    + transform->up() * mouseMovementSpeed * (float)mouseDelta.y
                    - transform->right() * mouseMovementSpeed * (float)mouseDelta.x );
        }
}

vec3 CameraController::rotateAroundPoint(vec3 input, vec3 pivotPoint){
    vec3 direction = input - pivotPoint;
    Spherical spherical = Spherical::cartesianToSpherical(direction);
    ivec2 mouseDelta = MouseInput::positionDelta();

    spherical.polar += mouseDelta.x * mouseRotationSpeed;
    spherical.elevation += mouseDelta.y * mouseRotationSpeed;
    return spherical.toCartesian() + pivotPoint;
}

void CameraController::orbit(){
    if (!target){
        return;
    }
    vec3 direction = transform->position() - target->position();
    vec3 virtualPivotPoint = transform->position() + transform->forward() * length(direction);
    vec3 rotatedVirtualPivotPoint = rotateAroundPoint(virtualPivotPoint, target->position());
    transform->setPosition(rotateAroundPoint(transform->position(), target->position()));
    transform->lookAt(rotatedVirtualPivotPoint);
}

void CameraController::dolly() {
    vec3 zoomDirection;
    if (zoomToMouse){
        zoomDirection = perspective->screenPointToRay(MouseInput::position()).direction();
    } else {
        zoomDirection = transform->forward();
    }
    transform->setPosition(transform->position() + zoomDirection * (mouseWheelSpeed * MouseInput::mouseWheelDelta().y));
}

void CameraController::setTarget(Transform *target){
    this->target = target;
}
