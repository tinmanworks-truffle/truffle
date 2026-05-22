#pragma once

#include "truffle/render/render_batch.hpp"
#include "truffle/rhi/rhi.hpp"

#include <memory>
#include <unordered_map>

namespace truffle::render {

// ---------------------------------------------------------------------------
// IPipelineCache — maps (InstanceLayout, MaterialId) → IPipeline
// ---------------------------------------------------------------------------

class IPipelineCache {
public:
    virtual ~IPipelineCache() = default;

    // Returns a matching pipeline or creates one. Returns nullptr on failure.
    [[nodiscard]] virtual rhi::IPipeline* get_or_create(
        const InstanceLayout& layout, MaterialId material) = 0;
};

// ---------------------------------------------------------------------------
// NullPipelineCache — Phase 2 placeholder; always returns a single null pipeline
// ---------------------------------------------------------------------------

class NullPipelineCache final : public IPipelineCache {
public:
    explicit NullPipelineCache(rhi::IDevice& device);

    [[nodiscard]] rhi::IPipeline* get_or_create(
        const InstanceLayout& layout, MaterialId material) override;

private:
    rhi::IDevice*                  device_   = nullptr;
    std::unique_ptr<rhi::IPipeline> pipeline_;
};

} // namespace truffle::render
