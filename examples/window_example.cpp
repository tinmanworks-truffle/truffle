#include "truffle/render/renderer.hpp"
#include "truffle/rhi/null_backend.hpp"
#include "truffle/window/window.hpp"

#include <iostream>

int main() {
    auto windowResult = truffle::window::create_headless_window({
        .title = "Truffle Optional Window Flow",
        .extent = {1280, 720},
    });
    if (!windowResult.ok()) {
        std::cerr << windowResult.status().message << '\n';
        return 1;
    }

    auto backend = truffle::rhi::create_null_backend();
    auto deviceResult = backend->create_device({});
    if (!deviceResult.ok()) {
        std::cerr << deviceResult.status().message << '\n';
        return 1;
    }

    auto device = std::move(deviceResult).value();
    const auto& window = *windowResult.value();
    auto surfaceResult = device->create_surface({
        .native = window.native_surface(),
        .initialExtent = window.desc().extent,
    });
    std::cout << "optional window surface: "
              << (surfaceResult.ok() ? "ok" : surfaceResult.status().message)
              << '\n';
    return surfaceResult.ok() ? 0 : 1;
}
