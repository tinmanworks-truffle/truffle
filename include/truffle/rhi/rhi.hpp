#pragma once

#include "truffle/core/config.hpp"
#include "truffle/core/status.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace truffle::rhi {

enum class BackendKind {
    null_backend,
    vulkan,
    direct3d,
    opengl,
    metal,
};

enum class QueueKind {
    graphics,
    compute,
    transfer,
};

enum class BufferUsage {
    vertex,
    index,
    uniform,
    storage,
    transfer_source,
    transfer_destination,
};

enum class TextureFormat {
    rgba8_unorm,
    bgra8_unorm,
    depth32_float,
};

enum class NativeSurfaceKind {
    headless,
    win32,
    wayland,
    xcb,
    cocoa_layer,
    external,
};

struct Capabilities {
    bool presentation = false;
    bool validation = false;
    std::uint32_t maxFramesInFlight = 1;
};

struct AdapterInfo {
    std::uint32_t id = 0;
    std::string name;
    BackendKind backend = BackendKind::null_backend;
    Capabilities capabilities;
};

struct DeviceDesc {
    std::uint32_t adapterId = 0;
    core::RuntimeConfig runtime;
};

struct Extent2D {
    std::uint32_t width = 1;
    std::uint32_t height = 1;
};

struct BufferDesc {
    std::size_t size = 0;
    BufferUsage usage = BufferUsage::vertex;
    std::string debugName;
};

struct TextureDesc {
    Extent2D extent;
    TextureFormat format = TextureFormat::rgba8_unorm;
    std::string debugName;
};

struct SamplerDesc {
    bool linear_filtering = true;
};

struct ShaderDesc {
    std::string entryPoint = "main";
    std::vector<std::byte> bytecode;
};

struct PipelineDesc {
    std::string debugName;
    bool depthTest = true;
};

struct NativeSurface {
    NativeSurfaceKind kind = NativeSurfaceKind::headless;
    void* handle = nullptr;
    void* display = nullptr;
};

struct SurfaceDesc {
    NativeSurface native;
    Extent2D initialExtent;
};

struct SwapchainDesc {
    Extent2D extent;
    TextureFormat format = TextureFormat::bgra8_unorm;
    std::uint32_t framesInFlight = 2;
};

struct FenceDesc {
    bool signaled = false;
};

class IBuffer {
public:
    virtual ~IBuffer() = default;
    [[nodiscard]] virtual const BufferDesc& desc() const noexcept = 0;
};

class ITexture {
public:
    virtual ~ITexture() = default;
    [[nodiscard]] virtual const TextureDesc& desc() const noexcept = 0;
};

class ISampler {
public:
    virtual ~ISampler() = default;
};

class IShader {
public:
    virtual ~IShader() = default;
};

class IPipeline {
public:
    virtual ~IPipeline() = default;
    [[nodiscard]] virtual const PipelineDesc& desc() const noexcept = 0;
};

class ISurface {
public:
    virtual ~ISurface() = default;
    [[nodiscard]] virtual const SurfaceDesc& desc() const noexcept = 0;
};

class ISwapchain {
public:
    virtual ~ISwapchain() = default;
    [[nodiscard]] virtual const SwapchainDesc& desc() const noexcept = 0;
    [[nodiscard]] virtual core::Status resize(Extent2D extent) = 0;
};

class IFence {
public:
    virtual ~IFence() = default;
    [[nodiscard]] virtual bool signaled() const noexcept = 0;
};

// ---------------------------------------------------------------------------
// Frame upload ring — N-buffered CPU-writable GPU-visible upload memory
// ---------------------------------------------------------------------------

struct FrameAllocation {
    IBuffer*    buffer    = nullptr; // backing buffer; nullptr signals exhaustion
    std::size_t offset    = 0;
    void*       mappedPtr = nullptr; // CPU-writable pointer into the buffer
    std::size_t size      = 0;

    [[nodiscard]] bool valid() const noexcept { return buffer != nullptr; }
};

class IFrameUploadRing {
public:
    virtual ~IFrameUploadRing() = default;

    // Allocate size bytes aligned to alignment from the current frame's range.
    // Returns an invalid FrameAllocation (buffer == nullptr) if the ring is full.
    [[nodiscard]] virtual FrameAllocation allocate(
        std::size_t size, std::size_t alignment = 16) = 0;

    // Advance to the next frame slot, reclaiming the oldest completed frame.
    // Call once per frame after verifying prior GPU work is complete.
    virtual void advance() = 0;

    [[nodiscard]] virtual std::uint32_t frames_in_flight() const noexcept = 0;
    [[nodiscard]] virtual std::size_t   capacity_per_frame() const noexcept = 0;
};

class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;
    [[nodiscard]] virtual core::Status begin() = 0;
    [[nodiscard]] virtual core::Status draw(std::uint32_t vertex_count) = 0;
    [[nodiscard]] virtual core::Status draw_instanced(
        std::uint32_t vertex_count, std::uint32_t instance_count) = 0;
    [[nodiscard]] virtual core::Status end() = 0;
    [[nodiscard]] virtual bool ready_for_submit() const noexcept = 0;
};

class IQueue {
public:
    virtual ~IQueue() = default;
    [[nodiscard]] virtual QueueKind kind() const noexcept = 0;
    [[nodiscard]] virtual core::Status submit(ICommandBuffer& command_buffer,
                                              IFence* signal_fence = nullptr) = 0;
};

class IDevice {
public:
    virtual ~IDevice() = default;

    [[nodiscard]] virtual const Capabilities& capabilities() const noexcept = 0;
    [[nodiscard]] virtual IQueue& queue(QueueKind kind) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<IBuffer>>
    create_buffer(const BufferDesc& desc) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<ITexture>>
    create_texture(const TextureDesc& desc) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<ISampler>>
    create_sampler(const SamplerDesc& desc) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<IShader>>
    create_shader(const ShaderDesc& desc) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<IPipeline>>
    create_pipeline(const PipelineDesc& desc) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<ISurface>>
    create_surface(const SurfaceDesc& desc) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<ISwapchain>>
    create_swapchain(ISurface& surface, const SwapchainDesc& desc) = 0;
    [[nodiscard]] virtual std::unique_ptr<ICommandBuffer> create_command_buffer() = 0;
    [[nodiscard]] virtual std::unique_ptr<IFence> create_fence(const FenceDesc& desc) = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<IFrameUploadRing>>
    create_upload_ring(std::uint32_t frames_in_flight,
                       std::size_t   capacity_per_frame) = 0;
};

class IBackend {
public:
    virtual ~IBackend() = default;

    [[nodiscard]] virtual BackendKind kind() const noexcept = 0;
    [[nodiscard]] virtual std::vector<AdapterInfo> enumerate_adapters() const = 0;
    [[nodiscard]] virtual core::Result<std::unique_ptr<IDevice>>
    create_device(const DeviceDesc& desc) = 0;
};

} // namespace truffle::rhi
