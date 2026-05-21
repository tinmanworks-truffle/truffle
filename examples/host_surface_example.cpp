#include "truffle/render/renderer.hpp"
#include "truffle/rhi/null_backend.hpp"

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

namespace {

using truffle::core::Status;
using truffle::ecs::Entity;
using truffle::ecs::World;
using truffle::render::Camera;
using truffle::render::Light;
using truffle::render::MeshRenderer;
using truffle::render::Renderer;
using truffle::render::SceneExtractor;
using truffle::render::Transform;
using truffle::rhi::BufferDesc;
using truffle::rhi::BufferUsage;
using truffle::rhi::Extent2D;
using truffle::rhi::NativeSurfaceKind;
using truffle::rhi::NullBackendStats;
using truffle::rhi::PipelineDesc;
using truffle::rhi::ShaderDesc;
using truffle::rhi::SurfaceDesc;
using truffle::rhi::SwapchainDesc;
using truffle::rhi::TextureDesc;

constexpr std::uint32_t frameCount = 6;
constexpr double fixedDeltaSeconds = 1.0 / 30.0;

struct SimulationVelocity {
    float unitsPerSecond = 0.0F;
};

struct ArcadePulse {
    float vertexRate = 0.0F;
};

[[nodiscard]] int fail(const std::string& context, const Status& status) {
    std::cerr << context << ": " << status.message << '\n';
    return 1;
}

[[nodiscard]] std::vector<World::System> makeSystems() {
    return {
        [](World& world, double deltaSeconds) {
            world.each<Transform, SimulationVelocity>(
                [deltaSeconds](Entity, Transform& transform,
                               SimulationVelocity& velocity) {
                    transform.worldMatrix[12] +=
                        velocity.unitsPerSecond * static_cast<float>(deltaSeconds);
                });
        },
        [](World& world, double deltaSeconds) {
            world.each<MeshRenderer, ArcadePulse>(
                [deltaSeconds](Entity, MeshRenderer& mesh, ArcadePulse& pulse) {
                    const auto pulseStep = static_cast<std::uint32_t>(
                        pulse.vertexRate * static_cast<float>(deltaSeconds));
                    mesh.vertexCount += pulseStep;
                });
        },
    };
}

void buildMixedScene(World& world) {
    const auto inspectionPart = world.create();
    world.emplace<Transform>(inspectionPart);
    world.emplace<MeshRenderer>(inspectionPart, MeshRenderer{10, 100, 24});

    const auto simulatedProbe = world.create();
    world.emplace<Transform>(simulatedProbe);
    world.emplace<MeshRenderer>(simulatedProbe, MeshRenderer{20, 200, 12});
    world.emplace<SimulationVelocity>(simulatedProbe, SimulationVelocity{3.0F});

    const auto arcadeActor = world.create();
    world.emplace<Transform>(arcadeActor);
    world.emplace<MeshRenderer>(arcadeActor, MeshRenderer{30, 300, 18});
    world.emplace<ArcadePulse>(arcadeActor, ArcadePulse{60.0F});

    const auto camera = world.create();
    world.emplace<Transform>(camera);
    world.emplace<Camera>(camera);

    const auto keyLight = world.create();
    world.emplace<Transform>(keyLight);
    world.emplace<Light>(keyLight, Light{2.0F});
}

[[nodiscard]] bool validStats(const NullBackendStats& stats) {
    return stats.buffersCreated == 1 && stats.texturesCreated == 1 &&
           stats.surfacesCreated == 1 && stats.swapchainsCreated == 1 &&
           stats.commandBuffersCreated == frameCount &&
           stats.drawsRecorded == frameCount * 3 &&
           stats.submissions == frameCount;
}

[[nodiscard]] int runHostOwnedConsumer() {
    auto backend = truffle::rhi::create_null_backend();
    const auto adapters = backend->enumerate_adapters();
    if (adapters.empty() || !adapters.front().capabilities.presentation) {
        std::cerr << "host app requires a presentable Truffle adapter\n";
        return 1;
    }

    auto deviceResult = backend->create_device({});
    if (!deviceResult.ok()) {
        return fail("create device", deviceResult.status());
    }
    auto device = std::move(deviceResult).value();

    auto vertexBufferResult = device->create_buffer(
        BufferDesc{.size = 4096, .usage = BufferUsage::vertex,
                   .debugName = "host-mixed-scene-vertices"});
    if (!vertexBufferResult.ok()) {
        return fail("create vertex buffer", vertexBufferResult.status());
    }
    auto textureResult = device->create_texture(
        TextureDesc{.extent = {512, 512}, .debugName = "host-viewport-albedo"});
    if (!textureResult.ok()) {
        return fail("create texture", textureResult.status());
    }
    auto shaderResult = device->create_shader(
        ShaderDesc{.entryPoint = "main",
                   .bytecode = {std::byte{0x54}, std::byte{0x52},
                                std::byte{0x46}}});
    if (!shaderResult.ok()) {
        return fail("create shader", shaderResult.status());
    }
    auto pipelineResult = device->create_pipeline(
        PipelineDesc{.debugName = "host-mixed-scene-pipeline"});
    if (!pipelineResult.ok()) {
        return fail("create pipeline", pipelineResult.status());
    }

    const Extent2D initialExtent{1024, 768};
    auto surfaceResult = device->create_surface(SurfaceDesc{
        .native = {.kind = NativeSurfaceKind::external},
        .initialExtent = initialExtent,
    });
    if (!surfaceResult.ok()) {
        return fail("create host surface", surfaceResult.status());
    }
    auto surface = std::move(surfaceResult).value();

    auto swapchainResult =
        device->create_swapchain(*surface, SwapchainDesc{.extent = initialExtent});
    if (!swapchainResult.ok()) {
        return fail("create swapchain", swapchainResult.status());
    }
    auto swapchain = std::move(swapchainResult).value();

    World world;
    buildMixedScene(world);
    const auto systems = makeSystems();
    SceneExtractor extractor;
    Renderer renderer{*device};

    for (std::uint32_t frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
        if (frameIndex == frameCount / 2) {
            if (const auto resizeStatus = swapchain->resize({1280, 720});
                !resizeStatus.ok()) {
                return fail("resize host swapchain", resizeStatus);
            }
        }

        world.run(systems, fixedDeltaSeconds);
        const auto frame = extractor.extract(world);
        if (frame.cameras.size() != 1 || frame.lights.size() != 1 ||
            frame.renderItems.size() != 3) {
            std::cerr << "mixed ECS scene extraction changed unexpectedly\n";
            return 1;
        }
        if (const auto renderStatus = renderer.render(frame); !renderStatus.ok()) {
            return fail("render host frame", renderStatus);
        }
    }

    const auto stats = backend->stats();
    if (!validStats(stats)) {
        std::cerr << "host app null backend stats changed unexpectedly\n";
        return 1;
    }

    std::cout << "host consumer frames=" << stats.submissions
              << " draws=" << stats.drawsRecorded
              << " resized=" << swapchain->desc().extent.width << 'x'
              << swapchain->desc().extent.height << '\n';
    return 0;
}

} // namespace

int main() {
    return runHostOwnedConsumer();
}
