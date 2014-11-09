//
// Created by Morten Nobel-Jørgensen on 07/11/14.
// Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//


#pragma once

#include "kick/kick.h"
#include "HMesh.h"

class HMeshConverter {
public:
    static void convert(HMesh* source, kick::MeshData *dest);
    static void convert(kick::MeshData *source, HMesh *dest);
};



