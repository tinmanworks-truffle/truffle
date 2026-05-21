#pragma once

#include "truffle/core/status.hpp"
#include "truffle/ecs/world.hpp"
#include "truffle/render/components.hpp"
#include "truffle/rhi/rhi.hpp"

#include <vector>

namespace truffle::render {

struct RenderItem {
    ecs::Entity entity;
    Transform transform;
    MeshRenderer mesh;
};

struct ExtractedFrame {
    std::vector<ecs::Entity> cameras;
    std::vector<ecs::Entity> lights;
    std::vector<RenderItem> renderItems;
};

class SceneExtractor {
public:
    [[nodiscard]] ExtractedFrame extract(ecs::World& world) const;
};

class Renderer {
public:
    explicit Renderer(rhi::IDevice& device);

    [[nodiscard]] core::Status render(const ExtractedFrame& frame);

private:
    rhi::IDevice* device_ = nullptr;
};

} // namespace truffle::render

