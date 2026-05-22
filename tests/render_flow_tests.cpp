#include "test_support.hpp"
#include "truffle/render/render_batch.hpp"
#include "truffle/render/renderer.hpp"
#include "truffle/rhi/null_backend.hpp"

#include <cstring>

int main() {
    auto backend = truffle::rhi::create_null_backend();
    auto deviceResult = backend->create_device({});
    TRUFFLE_CHECK(deviceResult.ok());
    auto device = std::move(deviceResult).value();

    // Create a 2-frame upload ring with 1 MiB per frame.
    auto ringResult = device->create_upload_ring(2, 1024 * 1024);
    TRUFFLE_CHECK(ringResult.ok());
    auto ring = std::move(ringResult).value();

    // Build an InstanceLayout with one Transform channel (SoA).
    truffle::render::InstanceLayout layout;
    layout.channels.push_back({
        truffle::render::ChannelKind::Transform,
        0, 0, 64,
    });
    layout.bindingModel = truffle::render::BindingModel::Separate;

    // Allocate ring memory for 3 instances × 64 bytes each.
    constexpr std::uint32_t kInstances  = 3;
    constexpr std::size_t   kStride     = 64;
    auto alloc = ring->allocate(kInstances * kStride);
    TRUFFLE_CHECK(alloc.valid());

    // Write identity matrices directly into ring memory (no intermediate copy).
    auto* dst = static_cast<float*>(alloc.mappedPtr);
    for (std::uint32_t i = 0; i < kInstances; ++i) {
        const float identity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        std::memcpy(dst + i * 16, identity, kStride);
    }

    truffle::render::RenderBatch batch;
    batch.layout        = layout;
    batch.instanceCount = kInstances;
    batch.vertexCount   = 3;
    batch.bindings[0]   = {alloc.buffer, alloc.offset, kInstances * kStride};
    batch.kind          = truffle::render::DrawKind::Direct;

    const truffle::render::RenderBatch batches[] = {batch};
    TRUFFLE_CHECK(truffle::render::Renderer{*device}.render(batches).ok());

    // One draw call per batch (instanced), one submission.
    TRUFFLE_CHECK(backend->stats().drawsRecorded == 1);
    TRUFFLE_CHECK(backend->stats().submissions == 1);
    return 0;
}
