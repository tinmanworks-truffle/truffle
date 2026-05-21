#include "truffle/render/renderer.hpp"
#include "truffle/rhi/null_backend.hpp"

#include <iostream>

int main() {
    auto backend = truffle::rhi::create_null_backend();
    auto deviceResult = backend->create_device({});
    if (!deviceResult.ok()) {
        std::cerr << deviceResult.status().message << '\n';
        return 1;
    }

    auto device = std::move(deviceResult).value();
    auto surfaceResult = device->create_surface({
        .native = {.kind = truffle::rhi::NativeSurfaceKind::external},
        .initialExtent = {800, 600},
    });
    if (!surfaceResult.ok()) {
        std::cerr << surfaceResult.status().message << '\n';
        return 1;
    }

    truffle::ecs::World world;
    const auto entity = world.create();
    world.emplace<truffle::render::Transform>(entity);
    world.emplace<truffle::render::MeshRenderer>(
        entity, truffle::render::MeshRenderer{1, 1, 36});

    const auto frame = truffle::render::SceneExtractor{}.extract(world);
    const auto status = truffle::render::Renderer{*device}.render(frame);
    std::cout << "host surface render submission: "
              << (status.ok() ? "ok" : status.message) << '\n';
    return status.ok() ? 0 : 1;
}
