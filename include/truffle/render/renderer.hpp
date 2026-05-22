#pragma once

#include "truffle/core/status.hpp"
#include "truffle/render/pipeline_cache.hpp"
#include "truffle/render/render_batch.hpp"
#include "truffle/rhi/rhi.hpp"

#include <span>

namespace truffle::render {

class Renderer {
public:
    explicit Renderer(rhi::IDevice& device, IPipelineCache* cache = nullptr);

    [[nodiscard]] core::Status render(std::span<const RenderBatch> batches);

private:
    rhi::IDevice*   device_ = nullptr;
    IPipelineCache* cache_  = nullptr;
};

} // namespace truffle::render

