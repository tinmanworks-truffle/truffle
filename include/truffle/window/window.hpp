#pragma once

#include "truffle/core/status.hpp"
#include "truffle/rhi/rhi.hpp"

#include <memory>
#include <string>

namespace truffle::window {

struct WindowDesc {
    std::string title = "Truffle";
    rhi::Extent2D extent{1280, 720};
};

class IWindow {
public:
    virtual ~IWindow() = default;

    [[nodiscard]] virtual const WindowDesc& desc() const noexcept = 0;
    [[nodiscard]] virtual bool open() const noexcept = 0;
    [[nodiscard]] virtual rhi::NativeSurface native_surface() const noexcept = 0;
    virtual void close() noexcept = 0;
};

// Headless windows prove optional window/surface flow before native helpers land.
[[nodiscard]] core::Result<std::unique_ptr<IWindow>>
create_headless_window(const WindowDesc& desc);

} // namespace truffle::window

