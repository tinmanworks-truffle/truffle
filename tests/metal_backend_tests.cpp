#include "test_support.hpp"
#include "truffle/rhi/metal_backend.hpp"

#include <cstring>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Minimal MSL shaders — fullscreen triangle, no vertex inputs
// ---------------------------------------------------------------------------

static const char kVertexMSL[] = R"msl(
#include <metal_stdlib>
using namespace metal;
vertex float4 vert_main(uint vid [[vertex_id]]) {
    const float2 pos[3] = {{-1,-1},{3,-1},{-1,3}};
    return float4(pos[vid % 3], 0.0, 1.0);
}
)msl";

static const char kFragmentMSL[] = R"msl(
#include <metal_stdlib>
using namespace metal;
fragment float4 frag_main() {
    return float4(0.5, 0.5, 0.5, 1.0);
}
)msl";

static std::vector<std::byte> to_bytes(const char* src) {
    const auto* p = reinterpret_cast<const std::byte*>(src);
    return {p, p + std::strlen(src)};
}

int main() {
    auto backend = truffle::rhi::create_metal_backend();

    // Skip gracefully when no Metal GPU is present (e.g. pure-CPU CI VMs)
    if (backend->enumerate_adapters().empty()) {
        return 0;
    }

    auto deviceResult = backend->create_device({});
    TRUFFLE_CHECK(deviceResult.ok());
    auto device = std::move(deviceResult).value();

    // --- Buffer ---
    TRUFFLE_CHECK(!device->create_buffer({}).ok()); // size==0 must fail
    auto bufResult = device->create_buffer({
        .size      = 256,
        .usage     = truffle::rhi::BufferUsage::vertex,
        .debugName = "triangle_vb",
    });
    TRUFFLE_CHECK(bufResult.ok());
    auto vb = std::move(bufResult).value();

    // --- Texture ---
    auto texResult = device->create_texture({
        .extent    = {64, 64},
        .format    = truffle::rhi::TextureFormat::rgba8_unorm,
        .debugName = "test_texture",
    });
    TRUFFLE_CHECK(texResult.ok());

    // --- Sampler ---
    TRUFFLE_CHECK(device->create_sampler({}).ok());

    // --- Shaders (MSL source as bytecode) ---
    auto vertResult = device->create_shader({
        .stage      = truffle::rhi::ShaderStage::vertex,
        .entryPoint = "vert_main",
        .bytecode   = to_bytes(kVertexMSL),
    });
    TRUFFLE_CHECK(vertResult.ok());
    auto vertShader = std::move(vertResult).value();

    auto fragResult = device->create_shader({
        .stage      = truffle::rhi::ShaderStage::fragment,
        .entryPoint = "frag_main",
        .bytecode   = to_bytes(kFragmentMSL),
    });
    TRUFFLE_CHECK(fragResult.ok());
    auto fragShader = std::move(fragResult).value();

    // Pipeline requires both shaders
    TRUFFLE_CHECK(!device->create_pipeline({.debugName = "no_shaders"}).ok());

    auto pipelineResult = device->create_pipeline({
        .debugName      = "test_pipeline",
        .vertexShader   = vertShader.get(),
        .fragmentShader = fragShader.get(),
    });
    TRUFFLE_CHECK(pipelineResult.ok());
    auto pipeline = std::move(pipelineResult).value();

    // --- Headless surface + swapchain ---
    auto surfaceResult = device->create_surface({
        .native        = {.kind = truffle::rhi::NativeSurfaceKind::headless},
        .initialExtent = {320, 240},
    });
    TRUFFLE_CHECK(surfaceResult.ok());
    auto surface = std::move(surfaceResult).value();

    auto swapchainResult = device->create_swapchain(
        *surface, {.extent = {320, 240}, .framesInFlight = 2});
    TRUFFLE_CHECK(swapchainResult.ok());
    auto swapchain = std::move(swapchainResult).value();

    // --- acquire_next_texture ---
    auto* drawable = swapchain->acquire_next_texture();
    TRUFFLE_CHECK(drawable != nullptr);

    // --- Full render pass sequence ---
    auto cmd = device->create_command_buffer();
    TRUFFLE_CHECK(cmd->begin().ok());

    truffle::rhi::RenderPassDesc passDesc;
    passDesc.extent                  = {320, 240};
    passDesc.colorAttachment.texture = drawable;
    TRUFFLE_CHECK(cmd->begin_render_pass(passDesc).ok());

    TRUFFLE_CHECK(cmd->bind_pipeline(*pipeline).ok());
    TRUFFLE_CHECK(cmd->bind_vertex_buffer(0, *vb).ok());
    TRUFFLE_CHECK(cmd->set_viewport(0, 0, 320, 240).ok());
    TRUFFLE_CHECK(cmd->set_scissor(0, 0, 320, 240).ok());
    TRUFFLE_CHECK(cmd->draw(3).ok());

    TRUFFLE_CHECK(cmd->end_render_pass().ok());
    TRUFFLE_CHECK(swapchain->schedule_present(*cmd).ok());
    TRUFFLE_CHECK(cmd->end().ok());

    TRUFFLE_CHECK(device->queue(truffle::rhi::QueueKind::graphics).submit(*cmd).ok());

    // --- Second frame: re-acquire + instanced draw ---
    auto* drawable2 = swapchain->acquire_next_texture();
    TRUFFLE_CHECK(drawable2 != nullptr);

    auto cmd2 = device->create_command_buffer();
    TRUFFLE_CHECK(cmd2->begin().ok());

    truffle::rhi::RenderPassDesc passDesc2;
    passDesc2.extent                  = {320, 240};
    passDesc2.colorAttachment.texture = drawable2;
    TRUFFLE_CHECK(cmd2->begin_render_pass(passDesc2).ok());
    TRUFFLE_CHECK(cmd2->bind_pipeline(*pipeline).ok());
    TRUFFLE_CHECK(cmd2->draw_instanced(3, 4).ok());
    TRUFFLE_CHECK(cmd2->end_render_pass().ok());
    TRUFFLE_CHECK(swapchain->schedule_present(*cmd2).ok());
    TRUFFLE_CHECK(cmd2->end().ok());
    TRUFFLE_CHECK(device->queue(truffle::rhi::QueueKind::graphics).submit(*cmd2).ok());

    // --- Frame upload ring ---
    auto ringResult = device->create_upload_ring(2, 512 * 1024);
    TRUFFLE_CHECK(ringResult.ok());
    auto ring = std::move(ringResult).value();

    auto alloc = ring->allocate(64);
    TRUFFLE_CHECK(alloc.valid());
    TRUFFLE_CHECK(alloc.mappedPtr != nullptr);
    // Write to the mapped region to verify CPU write access
    std::memset(alloc.mappedPtr, 0xFF, 64);
    ring->advance();

    // --- Fence ---
    auto fence = device->create_fence({});
    TRUFFLE_CHECK(!fence->signaled());

    auto cmd3 = device->create_command_buffer();
    TRUFFLE_CHECK(cmd3->begin().ok());
    TRUFFLE_CHECK(cmd3->end().ok());
    TRUFFLE_CHECK(device->queue(truffle::rhi::QueueKind::graphics)
                      .submit(*cmd3, fence.get())
                      .ok());
    fence->wait(); // async completion handler — block until GPU signals
    TRUFFLE_CHECK(fence->signaled());

    // --- Swapchain resize ---
    TRUFFLE_CHECK(swapchain->resize({640, 480}).ok());

    return 0;
}
