#pragma once

#include <array>
#include <cstdint>

namespace truffle::render {

struct Transform {
    std::array<float, 16> worldMatrix{
        1.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F,
    };
};

struct Camera {
    float verticalFieldOfViewRadians = 1.0472F;
    float nearPlane = 0.1F;
    float farPlane = 1000.0F;
};

struct MeshRenderer {
    std::uint64_t meshId = 0;
    std::uint64_t materialId = 0;
    std::uint32_t vertexCount = 3;
};

struct Light {
    float intensity = 1.0F;
};

} // namespace truffle::render

