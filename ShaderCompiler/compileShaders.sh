#!/usr/bin/env bash

glslc CrossPlatformGameEngine/src/shaders/GPUDrivenShader.vert -o CrossPlatformGameEngine/src/shaders/GPUDrivenVS.spv
glslc CrossPlatformGameEngine/src/shaders/GPUDrivenShader.frag -o CrossPlatformGameEngine/src/shaders/GPUDrivenPS.spv
glslc CrossPlatformGameEngine/src/shaders/GPUDrivenShader.comp -o CrossPlatformGameEngine/src/shaders/GPUDrivenCS.spv
