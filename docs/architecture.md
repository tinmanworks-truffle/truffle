# Architecture

## Layers

Truffle is one product with modules that stay linkable at different levels:

1. Foundation modules such as `truffle_core` provide shared status,
   configuration, and identity primitives.
2. `truffle_rhi` defines backend-neutral devices, queues, commands, resources,
   pipelines, surfaces, swapchains, synchronization, capabilities, and the
   `IFrameUploadRing` N-buffered upload primitive.
3. `truffle_backend_*` modules implement RHI contracts behind backend ownership.
   The null backend is buildable now; Vulkan, Direct3D, Metal, and OpenGL have
   reserved module and build-option boundaries for later milestones.
4. Rendering modules such as `truffle_render` expose `RenderBatch`,
   `InstanceLayout`, `Renderer`, and the `IPipelineCache` interface. This layer
   has zero compile-time dependency on `truffle_ecs`.
5. The optional `truffle_scene` module bridges ECS worlds into render batches.
   It depends on both `truffle_ecs` and `truffle_render` and provides
   `SceneAdapter`, which extracts a `SceneFrame` containing camera state, light
   state, and a vector of `RenderBatch` objects written through
   `IFrameUploadRing`.
6. Future framework-facing modules can add assets, debug hooks, and tool-facing
   rendering workflows above the lower layers without hiding them from consumers.

## Repository Shape

Public contracts stay under `include/truffle`. Runtime implementations stay
under module-owned `src` directories, with backend code under `src/backends`
and the scene adapter under `src/scene`. CMake helpers under `cmake` own
options, warnings, formatting hooks, install rules, and package export behavior.
Examples and tests remain consumer proof and validation layers instead of
runtime dependencies.

## Three Consumption Levels

### Level 1 — Graphics API consumer
Link `truffle_core`, `truffle_rhi`, and one backend. Manage all GPU resources
directly. No ECS or scene abstraction required.

### Level 2 — Render module consumer
Add `truffle_render`. Build `RenderBatch` objects directly from any data source
(typed arrays, streaming buffers, or GPU-resident handles) and call
`Renderer::render()`. Use `IFrameUploadRing` for CPU-to-GPU uploads.

### Level 3 — Full framework consumer
Add `truffle_ecs` and `truffle_scene`. Populate an ECS world and call
`SceneAdapter::extract(world, ring)` each frame. The adapter writes transform
data through the ring and returns a `SceneFrame` with ready-to-render batches.

## Data Flow — Three Lanes

```
Lane A — ECS extraction (truffle_scene)
  ecs::World → SceneAdapter → RenderBatch[] via IFrameUploadRing
  Good for: normal scenes, cameras, lights, standard scene workflows

Lane B — Bulk direct upload (caller-owned)
  RawBuffer<T> → IFrameUploadRing allocation → RenderBatch
  Good for: million detections, point clouds, dense simulation outputs

Lane C — GPU-resident (future)
  IBuffer handle → RenderBatch (zero upload cost)
  Good for: static geometry, GPU-generated data, persistent simulation state
```

All three lanes produce `RenderBatch` objects. `Renderer::render()` accepts a
`std::span<const RenderBatch>` and is agnostic to which lane produced them.

`InstanceLayout` declares at runtime which channels are present (`Transform`,
`Color`, `Normal`, `TexCoord`, etc.), which buffer binding each occupies, and
the per-instance stride. A `BindingModel` flag chooses Separate (SoA, default)
or Interleaved (AoS). `InstanceLayout::hash()` keys pipeline cache lookup and
future shader permutation selection.

## GPU Transform Hierarchy (future — Phase 3/4)

Large hierarchies resolve world matrices on the GPU via a compute pass over
`ChannelKind::LocalTransform` and `ChannelKind::ParentIndex` arrays. The
channel kinds are reserved in the current `InstanceLayout` enum. The compute
pass design and dispatch API are Phase 3/4 work.

## Host Boundary

Truffle does not own native windowing, input policy, application lifetime, or
the consumer simulation model. Consumers provide host loops and native surface
boundaries, then choose whether to link low-level RHI modules, renderer modules,
scene adapter modules, or future higher-level framework modules.

## Dependency Boundary

Runtime dependencies prefer Git submodules when that is practical. Pinned
source copies are the fallback when submodules do not fit, and they must retain
license, provenance, and a narrow build footprint. Example-only dependencies
stay inside their example folders so they do not become Truffle runtime
dependencies. The `window` module boundary is reserved but unpopulated; Truffle
does not currently own windowing helpers.

## Current Baseline

The repository establishes its Phase 2 render data foundation. `truffle_render`
is ECS-independent; `RenderBatch` and `InstanceLayout` are the universal
renderer input contract; `IFrameUploadRing` is the N-buffered CPU-to-GPU upload
primitive; `truffle_scene` provides optional ECS extraction via `SceneAdapter`;
and `IPipelineCache` defines the interface for future shader variant management.
ADRs 0004-0006 lock these design decisions before production renderer APIs land.
