# Roadmap

## Phase 1: Foundation And Contracts — Complete

- Establish modular CMake targets and public include layout.
- Make current library layers installable and consumable through CMake package
  exports.
- Add developer presets, quality checks, and cross-platform CI workflow files.
- Validate ECS entity, component, query, and system boundaries.
- Validate RHI resource, command, surface, swapchain, and submission contracts
  through the null backend.
- Prove host-owned surface embedding with a structured workspace example.

## Phase 2: Render Data Foundation — In Progress

- Define `RenderBatch` and `InstanceLayout` as the universal renderer input
  contract, decoupling `truffle_render` from `truffle_ecs`.
- Establish `IFrameUploadRing` as the N-buffered CPU-to-GPU upload primitive at
  the RHI layer, supporting both ECS extraction and direct bulk upload.
- Introduce `truffle_scene` as the optional ECS-to-render bridge module.
- Reserve GPU transform hierarchy direction for Phase 3/4 compute pass work.
- Define `IPipelineCache` interface for future shader variant management.
- Expand ECS component definitions for cameras, lighting, meshes, and materials.

## Phase 3: Rendering Framework Layers

- Add higher rendering layers for materials, asset-facing boundaries, debug
  hooks, and tool-facing orchestration without hiding low-level modules.
- Implement GPU transform hierarchy compute pass for large node graphs.
- Keep scene adapters optional so consumers can choose Truffle ECS or their own
  data source.
- Grow frame graph and render pass orchestration from the current submission
  baseline.
- Add shader reflection and binding layout direction without binding the public
  API to one GPU backend.

## Phase 4: Production Backends

- Implement Vulkan for Windows and Linux.
- Implement Direct3D for Windows.
- Implement Metal for macOS.
- Add OpenGL where compatibility and inspection workloads benefit from it.
