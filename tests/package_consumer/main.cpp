#include "truffle/rhi/null_backend.hpp"
#include "truffle/scene/scene_adapter.hpp"

int main() {
    auto backend = truffle::rhi::create_null_backend();
    auto deviceResult = backend->create_device({});
    if (!deviceResult.ok()) return 1;
    auto device = std::move(deviceResult).value();

    auto ringResult = device->create_upload_ring(2, 256 * 1024);
    if (!ringResult.ok()) return 1;
    auto ring = std::move(ringResult).value();

    truffle::ecs::World world;
    const auto e = world.create();
    world.emplace<truffle::render::Transform>(e);
    world.emplace<truffle::render::MeshRenderer>(e);

    truffle::scene::SceneAdapter adapter;
    auto frame = adapter.extract(world, *ring);
    return (frame.meshBatches.size() == 1) ? 0 : 1;
}
