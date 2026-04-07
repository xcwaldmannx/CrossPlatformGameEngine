#!/usr/bin/env bash

glslc CrossPlatformGameEngine/src/shaders/GPUDrivenShader.vert -o CrossPlatformGameEngine/src/shaders/GPUDrivenVS.spv
glslc CrossPlatformGameEngine/src/shaders/GPUDrivenShader.frag -o CrossPlatformGameEngine/src/shaders/GPUDrivenPS.spv
glslc CrossPlatformGameEngine/src/shaders/GPUDrivenShader.comp -o CrossPlatformGameEngine/src/shaders/GPUDrivenCS.spv

glslc CrossPlatformGameEngine/src/shaders/Simple/FrustumCullingShader.comp -o CrossPlatformGameEngine/src/shaders/Simple/FrustumCullingShader.spv
glslc CrossPlatformGameEngine/src/shaders/Simple/VertexShader.vert -o CrossPlatformGameEngine/src/shaders/Simple/VertexShader.spv
glslc CrossPlatformGameEngine/src/shaders/Simple/PixelShader.frag -o CrossPlatformGameEngine/src/shaders/Simple/PixelShader.spv
glslc CrossPlatformGameEngine/src/shaders/Simple/BoundsVertexShader.vert -o CrossPlatformGameEngine/src/shaders/Simple/BoundsVertexShader.spv
glslc CrossPlatformGameEngine/src/shaders/Simple/BoundsPixelShader.frag -o CrossPlatformGameEngine/src/shaders/Simple/BoundsPixelShader.spv

glslc CrossPlatformGameEngine/src/shaders/BoundingBoxCS.comp -o CrossPlatformGameEngine/src/shaders/BoundingBoxCS.spv
glslc CrossPlatformGameEngine/src/shaders/BoundingBoxVS.vert -o CrossPlatformGameEngine/src/shaders/BoundingBoxVS.spv
glslc CrossPlatformGameEngine/src/shaders/BoundingBoxPS.frag -o CrossPlatformGameEngine/src/shaders/BoundingBoxPS.spv
