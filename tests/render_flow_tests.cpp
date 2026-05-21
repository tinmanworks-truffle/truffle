#include "test_support.hpp"
#include "truffle/render/renderer.hpp"
#include "truffle/rhi/null_backend.hpp"

int main() {
    truffle::ecs::World world;

    const auto camera = world.create();
    world.emplace<truffle::render::Transform>(camera);
    world.emplace<truffle::render::Camera>(camera);

    const auto light = world.create();
    world.emplace<truffle::render::Transform>(light);
    world.emplace<truffle::render::Light>(light);

    const auto mesh = world.create();
    world.emplace<truffle::render::Transform>(mesh);
    world.emplace<truffle::render::MeshRenderer>(
        mesh, truffle::render::MeshRenderer{7, 4, 12});

    const auto frame = truffle::render::SceneExtractor{}.extract(world);
    TRUFFLE_CHECK(frame.cameras.size() == 1);
    TRUFFLE_CHECK(frame.lights.size() == 1);
    TRUFFLE_CHECK(frame.renderItems.size() == 1);

    auto backend = truffle::rhi::create_null_backend();
    auto deviceResult = backend->create_device({});
    TRUFFLE_CHECK(deviceResult.ok());
    auto device = std::move(deviceResult).value();

    TRUFFLE_CHECK(truffle::render::Renderer{*device}.render(frame).ok());
    TRUFFLE_CHECK(backend->stats().drawsRecorded == 1);
    TRUFFLE_CHECK(backend->stats().submissions == 1);
    return 0;
}
