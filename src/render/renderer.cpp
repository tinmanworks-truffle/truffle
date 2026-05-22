#include "truffle/render/pipeline_cache.hpp"
#include "truffle/render/renderer.hpp"

namespace truffle::render {

// ---------------------------------------------------------------------------
// NullPipelineCache
// ---------------------------------------------------------------------------

NullPipelineCache::NullPipelineCache(rhi::IDevice& device) : device_(&device) {}

rhi::IPipeline* NullPipelineCache::get_or_create(const InstanceLayout& /*layout*/,
                                                  MaterialId /*material*/) {
    if (!pipeline_) {
        auto result = device_->create_pipeline({.debugName = "null_pipeline"});
        if (result.ok()) {
            pipeline_ = std::move(result).value();
        }
    }
    return pipeline_.get();
}

// ---------------------------------------------------------------------------
// Renderer
// ---------------------------------------------------------------------------

Renderer::Renderer(rhi::IDevice& device, IPipelineCache* cache)
    : device_(&device), cache_(cache) {}

core::Status Renderer::render(std::span<const RenderBatch> batches) {
    auto commandBuffer = device_->create_command_buffer();
    if (const auto s = commandBuffer->begin(); !s.ok()) {
        return s;
    }

    for (const auto& batch : batches) {
        if (cache_) {
            (void)cache_->get_or_create(batch.layout, batch.material);
        }
        if (const auto s = commandBuffer->draw_instanced(
                batch.vertexCount, batch.instanceCount);
            !s.ok()) {
            return s;
        }
    }

    if (const auto s = commandBuffer->end(); !s.ok()) {
        return s;
    }
    return device_->queue(rhi::QueueKind::graphics).submit(*commandBuffer);
}

} // namespace truffle::render

