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

        const int32_t dist = 8;
        const int32_t diameter = dist * 2;
        const size_t chunkCount = diameter * diameter;
        const int32_t radius = dist;
        const int32_t radiusSquared = radius * radius;

        std::vector<TerrainData> chunks(chunkCount);

        const unsigned int threadCount = std::min(
            std::max(1u, std::thread::hardware_concurrency()),
            static_cast<unsigned int>(chunkCount)
        );

        const auto calculateLod = [&](int32_t x, int32_t z)
        {
            const int32_t d2 = x * x + z * z;
            uint32_t lod = 0;

            if (d2 >= 4)
            {
                const float distance = std::sqrt(static_cast<float>(d2));
                lod = static_cast<uint32_t>(std::floor(std::log2(distance)));
            }

            return std::min(lod, terrain.mMaxLod);
        };

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

                    const int32_t distanceSquared = x * x + z * z;
                    if (distanceSquared > radiusSquared) continue;

                    const int32_t locX = cameraChunkX + x;
                    const int32_t locZ = cameraChunkZ + z;

                    const auto lod = calculateLod(x, z);

                    const auto neighborLod = [&](int32_t nx, int32_t nz)
                    {
                        if (nx * nx + nz * nz > radiusSquared)
                            return lod;

                        return calculateLod(nx, nz);
                    };

                    const TerrainNeighborLods neighbors
                    {
                        .mLeft  = neighborLod(x - 1, z),
                        .mRight = neighborLod(x + 1, z),
                        .mDown  = neighborLod(x, z - 1),
                        .mUp    = neighborLod(x, z + 1)
                    };

                    TerrainGenerator::generate(
                        { locX, locZ },
                        terrain.mChunkSize,
                        terrain.mChunkHeight,
                        terrain.mChunkResolution,
                        lod,
                        0.0025f,
                        0.1f,
                        neighbors,
                        chunks[i]
                    );
                }
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        for (const TerrainData& data : chunks)
        {
            if (data.mVertices.empty() || data.mIndices.empty()) continue;

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
