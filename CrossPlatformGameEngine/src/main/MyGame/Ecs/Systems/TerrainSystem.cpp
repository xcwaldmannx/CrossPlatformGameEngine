#include "TerrainSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Components/CameraComponent.h"
#include "../Components/TerrainComponent.h"
#include "../Components/PhysicsComponent.h"

#include "../../Terrain/TerrainGenerator.h"

#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>

TerrainSystem::TerrainSystem(ascen::Engine& engine) : mEngine(engine) {}

void TerrainSystem::update(const float delta)
{
    std::vector<TerrainVertex> terrainVertices;
    std::vector<uint32_t> terrainIndices;
    std::vector<ascen::IndexedIndirectDraw> drawCalls;

    CameraComponent camera;
    if (!mSystem->getSystem<CameraSystem>()->getActiveCamera(camera)) return;

    for (auto& entity : mEntities)
    {
        auto& terrain = mSystem->getComponent<TerrainComponent>(entity);

        if (!terrain.mIsActive) continue;

        const auto chunkSize = static_cast<int32_t>(terrain.mChunkSize);
        const double halfChunkSize = chunkSize * 0.5;
        const auto cameraChunkX = static_cast<int32_t>(std::floor((static_cast<double>(camera.mPosition.x) + halfChunkSize) / chunkSize));
        const auto cameraChunkZ = static_cast<int32_t>(std::floor((static_cast<double>(camera.mPosition.z) + halfChunkSize) / chunkSize));

        if (mLastChunk == glm::ivec2(cameraChunkX, cameraChunkZ)) continue;
        mLastChunk = glm::ivec2(cameraChunkX, cameraChunkZ);

        printf("cx=%f, cz=%f", camera.mPosition.x, camera.mPosition.z);

        const int32_t dist = 4;
        const int32_t diameter = dist * 2;
        const size_t chunkCount = diameter * diameter;

        std::vector<TerrainData> chunks(chunkCount);

        const unsigned int threadCount = std::min(
            std::max(1u, std::thread::hardware_concurrency()),
            static_cast<unsigned int>(chunkCount)
        );

        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        for (unsigned int t = 0; t < threadCount; ++t)
        {
            threads.emplace_back([&, t]()
            {
                for (size_t i = t; i < chunkCount; i += threadCount)
                {
                    const int32_t x = static_cast<int32_t>(i / diameter) - dist;
                    const int32_t z = static_cast<int32_t>(i % diameter) - dist;

                    const int32_t locX = cameraChunkX + x;
                    const int32_t locZ = cameraChunkZ + z;

                    TerrainGenerator::generate(
                        { locX, locZ },
                        terrain.mChunkSize,
                        terrain.mChunkHeight,
                        terrain.mChunkResolution,
                        0.00005f,
                        0.005f,
                        chunks[i]);
                }
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        for (const TerrainData& data : chunks)
        {
            const auto vertexOffset = static_cast<uint32_t>(terrainVertices.size());
            const auto indexOffset = static_cast<uint32_t>(terrainIndices.size());

            ascen::IndexedIndirectDraw drawCall{};
            drawCall.indexCount = static_cast<uint32_t>(data.mIndices.size());
            drawCall.instanceCount = 1;
            drawCall.firstIndex = indexOffset;
            drawCall.vertexOffset = static_cast<int32_t>(vertexOffset);
            drawCall.firstInstance = 0;

            drawCalls.push_back(drawCall);

            terrainVertices.insert(terrainVertices.end(), data.mVertices.begin(), data.mVertices.end());
            terrainIndices.insert(terrainIndices.end(), data.mIndices.begin(), data.mIndices.end());
        }
    }


    if (!drawCalls.empty())
    {
        mEngine.resizeBuffer("BUFFER_VERTEX_TERRAIN", terrainVertices.size());
        mEngine.resizeBuffer("BUFFER_INDEX_TERRAIN", terrainIndices.size());
        mEngine.resizeBuffer("BUFFER_DRAWCALL_TERRAIN", drawCalls.size());

        mEngine.uploadBuffer("BUFFER_VERTEX_TERRAIN", terrainVertices.data(), terrainVertices.size(), sizeof(TerrainVertex), 0);
        mEngine.uploadBuffer("BUFFER_INDEX_TERRAIN", terrainIndices.data(), terrainIndices.size(), sizeof(uint32_t), 0);
        mEngine.uploadBuffer("BUFFER_DRAWCALL_TERRAIN", drawCalls.data(), drawCalls.size(), sizeof(ascen::IndexedIndirectDraw), 0);
    }


        // Mesh mesh;
        // mesh.mVertices.reserve(terrainVertices.size());
        // mesh.mIndices.reserve(terrainIndices.size());
        //
        // for (const auto& vertex : terrainVertices)
        // {
        //     mesh.mVertices.push_back(vertex.mPosition);
        // }
        //
        // for (const auto& index : terrainIndices)
        // {
        //     mesh.mIndices.push_back(static_cast<int32_t>(index));
        // }
        //
        // physics.mCollider = mesh;
        //
        // terrain.mIsLoaded = true;
}
