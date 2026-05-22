#include "test_support.hpp"
#include "truffle/rhi/null_backend.hpp"

int main() {
    auto backend = truffle::rhi::create_null_backend();
    TRUFFLE_CHECK(backend->enumerate_adapters().size() == 1);

    auto deviceResult = backend->create_device({});
    TRUFFLE_CHECK(deviceResult.ok());
    auto device = std::move(deviceResult).value();

    TRUFFLE_CHECK(!device->create_buffer({}).ok());
    auto bufferResult = device->create_buffer({
        .size = 1024,
        .usage = truffle::rhi::BufferUsage::vertex,
        .debugName = "vertex data",
    });
    TRUFFLE_CHECK(bufferResult.ok());

    auto surfaceResult = device->create_surface({
        .native = {.kind = truffle::rhi::NativeSurfaceKind::headless},
        .initialExtent = {640, 480},
    });
    TRUFFLE_CHECK(surfaceResult.ok());
    auto surface = std::move(surfaceResult).value();

    auto swapchainResult = device->create_swapchain(
        *surface, {.extent = {640, 480}, .framesInFlight = 2});
    TRUFFLE_CHECK(swapchainResult.ok());
    auto swapchain = std::move(swapchainResult).value();
    TRUFFLE_CHECK(swapchain->resize({1280, 720}).ok());

    // Verify swapchain acquire
    auto* drawable = swapchain->acquire_next_texture();
    TRUFFLE_CHECK(drawable != nullptr);

    // Full enriched command sequence
    auto commandBuffer = device->create_command_buffer();
    TRUFFLE_CHECK(commandBuffer->begin().ok());

    truffle::rhi::RenderPassDesc passDesc;
    passDesc.extent                  = {640, 480};
    passDesc.colorAttachment.texture = drawable;
    TRUFFLE_CHECK(commandBuffer->begin_render_pass(passDesc).ok());

    auto pipelineResult = device->create_pipeline({.debugName = "test_pipeline"});
    TRUFFLE_CHECK(pipelineResult.ok());
    auto pipeline = std::move(pipelineResult).value();
    TRUFFLE_CHECK(commandBuffer->bind_pipeline(*pipeline).ok());

    auto vbResult = device->create_buffer({
        .size = 256,
        .usage = truffle::rhi::BufferUsage::vertex,
        .debugName = "vb",
    });
    TRUFFLE_CHECK(vbResult.ok());
    auto vb = std::move(vbResult).value();
    TRUFFLE_CHECK(commandBuffer->bind_vertex_buffer(0, *vb).ok());
    TRUFFLE_CHECK(commandBuffer->set_viewport(0, 0, 640, 480).ok());
    TRUFFLE_CHECK(commandBuffer->set_scissor(0, 0, 640, 480).ok());
    TRUFFLE_CHECK(commandBuffer->draw(3).ok());

    TRUFFLE_CHECK(commandBuffer->end_render_pass().ok());
    TRUFFLE_CHECK(commandBuffer->end().ok());

    auto fence = device->create_fence({});
    TRUFFLE_CHECK(device->queue(truffle::rhi::QueueKind::graphics)
                      .submit(*commandBuffer, fence.get())
                      .ok());
    TRUFFLE_CHECK(fence->signaled());

    const auto stats = backend->stats();
    TRUFFLE_CHECK(stats.buffersCreated == 2); // original vb + new vb
    TRUFFLE_CHECK(stats.surfacesCreated == 1);
    TRUFFLE_CHECK(stats.swapchainsCreated == 1);
    TRUFFLE_CHECK(stats.drawsRecorded == 1);
    TRUFFLE_CHECK(stats.submissions == 1);
    return 0;
}
