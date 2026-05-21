#include "truffle/render/renderer.hpp"
#include "truffle/rhi/null_backend.hpp"
#include "truffle/window/window.hpp"

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
using truffle::rhi::NullBackendStats;
using truffle::rhi::PipelineDesc;
using truffle::rhi::ShaderDesc;
using truffle::rhi::SurfaceDesc;
using truffle::rhi::SwapchainDesc;
using truffle::rhi::TextureDesc;
using truffle::window::IWindow;
using truffle::window::WindowDesc;

constexpr std::uint32_t frameCount = 5;
constexpr double fixedDeltaSeconds = 1.0 / 60.0;

struct LinearDrift {
    float unitsPerSecond = 0.0F;
};

struct DetailPulse {
    float vertexRate = 0.0F;
};

[[nodiscard]] int fail(const std::string& context, const Status& status) {
    std::cerr << context << ": " << status.message << '\n';
    return 1;
}

[[nodiscard]] std::vector<World::System> makeSystems() {
    return {
        [](World& world, double deltaSeconds) {
            world.each<Transform, LinearDrift>(
                [deltaSeconds](Entity, Transform& transform, LinearDrift& drift) {
                    transform.worldMatrix[13] +=
                        drift.unitsPerSecond * static_cast<float>(deltaSeconds);
                });
        },
        [](World& world, double deltaSeconds) {
            world.each<MeshRenderer, DetailPulse>(
                [deltaSeconds](Entity, MeshRenderer& mesh, DetailPulse& pulse) {
                    mesh.vertexCount += static_cast<std::uint32_t>(
                        pulse.vertexRate * static_cast<float>(deltaSeconds));
                });
        },
    };
}

void buildMixedScene(World& world) {
    const auto importedSolid = world.create();
    world.emplace<Transform>(importedSolid);
    world.emplace<MeshRenderer>(importedSolid, MeshRenderer{101, 501, 30});

    const auto simulatedMarker = world.create();
    world.emplace<Transform>(simulatedMarker);
    world.emplace<MeshRenderer>(simulatedMarker, MeshRenderer{102, 502, 9});
    world.emplace<LinearDrift>(simulatedMarker, LinearDrift{1.5F});

    const auto interactiveActor = world.create();
    world.emplace<Transform>(interactiveActor);
    world.emplace<MeshRenderer>(interactiveActor, MeshRenderer{103, 503, 15});
    world.emplace<DetailPulse>(interactiveActor, DetailPulse{120.0F});

    const auto viewportCamera = world.create();
    world.emplace<Transform>(viewportCamera);
    world.emplace<Camera>(viewportCamera);

    const auto fillLight = world.create();
    world.emplace<Transform>(fillLight);
    world.emplace<Light>(fillLight, Light{1.25F});
}

[[nodiscard]] bool validStats(const NullBackendStats& stats) {
    return stats.buffersCreated == 1 && stats.texturesCreated == 1 &&
           stats.surfacesCreated == 1 && stats.swapchainsCreated == 1 &&
           stats.commandBuffersCreated == frameCount &&
           stats.drawsRecorded == frameCount * 3 &&
           stats.submissions == frameCount;
}

[[nodiscard]] int runWindowConsumer() {
    auto windowResult = truffle::window::create_headless_window(
        WindowDesc{.title = "Truffle Mixed Consumer", .extent = {960, 540}});
    if (!windowResult.ok()) {
        return fail("create Truffle window", windowResult.status());
    }
    auto window = std::move(windowResult).value();

    auto backend = truffle::rhi::create_null_backend();
    const auto adapters = backend->enumerate_adapters();
    if (adapters.empty() || !adapters.front().capabilities.presentation) {
        std::cerr << "window app requires a presentable Truffle adapter\n";
        return 1;
    }

    auto deviceResult = backend->create_device({});
    if (!deviceResult.ok()) {
        return fail("create device", deviceResult.status());
    }
    auto device = std::move(deviceResult).value();

    auto vertexBufferResult = device->create_buffer(
        BufferDesc{.size = 2048, .usage = BufferUsage::vertex,
                   .debugName = "window-mixed-scene-vertices"});
    if (!vertexBufferResult.ok()) {
        return fail("create vertex buffer", vertexBufferResult.status());
    }
    auto textureResult = device->create_texture(
        TextureDesc{.extent = {256, 256}, .debugName = "window-scene-swatch"});
    if (!textureResult.ok()) {
        return fail("create texture", textureResult.status());
    }
    auto shaderResult = device->create_shader(
        ShaderDesc{.entryPoint = "main",
                   .bytecode = {std::byte{0x54}, std::byte{0x57},
                                std::byte{0x4E}}});
    if (!shaderResult.ok()) {
        return fail("create shader", shaderResult.status());
    }
    auto pipelineResult = device->create_pipeline(
        PipelineDesc{.debugName = "window-mixed-scene-pipeline"});
    if (!pipelineResult.ok()) {
        return fail("create pipeline", pipelineResult.status());
    }

    const IWindow& graphicsWindow = *window;
    auto surfaceResult = device->create_surface(SurfaceDesc{
        .native = graphicsWindow.native_surface(),
        .initialExtent = graphicsWindow.desc().extent,
    });
    if (!surfaceResult.ok()) {
        return fail("create Truffle window surface", surfaceResult.status());
    }
    auto surface = std::move(surfaceResult).value();

    auto swapchainResult = device->create_swapchain(
        *surface, SwapchainDesc{.extent = graphicsWindow.desc().extent});
    if (!swapchainResult.ok()) {
        return fail("create swapchain", swapchainResult.status());
    }
    auto swapchain = std::move(swapchainResult).value();

    World world;
    buildMixedScene(world);
    const auto systems = makeSystems();
    SceneExtractor extractor;
    Renderer renderer{*device};

    std::uint32_t renderedFrames = 0;
    while (graphicsWindow.open() && renderedFrames < frameCount) {
        if (renderedFrames == 2) {
            if (const auto resizeStatus = swapchain->resize(Extent2D{1280, 800});
                !resizeStatus.ok()) {
                return fail("resize window swapchain", resizeStatus);
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
            return fail("render window frame", renderStatus);
        }
        ++renderedFrames;
    }
    window->close();

    const auto stats = backend->stats();
    if (window->open() || !validStats(stats)) {
        std::cerr << "window app lifecycle or null backend stats changed\n";
        return 1;
    }

    std::cout << "window consumer frames=" << stats.submissions
              << " draws=" << stats.drawsRecorded
              << " resized=" << swapchain->desc().extent.width << 'x'
              << swapchain->desc().extent.height << '\n';
    return 0;
}

} // namespace

int main() {
    return runWindowConsumer();
}
