#include "truffle/window/window.hpp"

#include <utility>

namespace truffle::window {
namespace {

class HeadlessWindow final : public IWindow {
public:
    explicit HeadlessWindow(WindowDesc desc) : desc_(std::move(desc)) {}

    [[nodiscard]] const WindowDesc& desc() const noexcept override { return desc_; }
    [[nodiscard]] bool open() const noexcept override { return open_; }

    [[nodiscard]] rhi::NativeSurface native_surface() const noexcept override {
        return rhi::NativeSurface{rhi::NativeSurfaceKind::headless, nullptr, nullptr};
    }

    void close() noexcept override { open_ = false; }

private:
    WindowDesc desc_;
    bool open_ = true;
};

} // namespace

core::Result<std::unique_ptr<IWindow>> create_headless_window(const WindowDesc& desc) {
    if (desc.extent.width == 0 || desc.extent.height == 0) {
        return core::Status::failure(core::StatusCode::invalid_argument,
                                     "window extent must be non-zero");
    }
    return std::unique_ptr<IWindow>(std::make_unique<HeadlessWindow>(desc));
}

} // namespace truffle::window

