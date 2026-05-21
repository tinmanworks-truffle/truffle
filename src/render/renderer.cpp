#include "truffle/render/renderer.hpp"

namespace truffle::render {

ExtractedFrame SceneExtractor::extract(ecs::World& world) const {
    ExtractedFrame frame;

    world.each<Transform, Camera>(
        [&frame](ecs::Entity entity, Transform&, Camera&) {
            frame.cameras.push_back(entity);
        });
    world.each<Transform, Light>(
        [&frame](ecs::Entity entity, Transform&, Light&) {
            frame.lights.push_back(entity);
        });
    world.each<Transform, MeshRenderer>(
        [&frame](ecs::Entity entity, Transform& transform, MeshRenderer& mesh) {
            frame.renderItems.push_back(RenderItem{entity, transform, mesh});
        });

    return frame;
}

Renderer::Renderer(rhi::IDevice& device) : device_(&device) {}

core::Status Renderer::render(const ExtractedFrame& frame) {
    auto commandBuffer = device_->create_command_buffer();
    if (const auto status = commandBuffer->begin(); !status.ok()) {
        return status;
    }

    for (const auto& renderItem : frame.renderItems) {
        if (const auto status = commandBuffer->draw(renderItem.mesh.vertexCount);
            !status.ok()) {
            return status;
        }
    }

    if (const auto status = commandBuffer->end(); !status.ok()) {
        return status;
    }
    return device_->queue(rhi::QueueKind::graphics).submit(*commandBuffer);
}

} // namespace truffle::render

