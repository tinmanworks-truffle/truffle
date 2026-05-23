# Roadmap

## Phase 1: Foundation And Contracts — Complete

- Establish modular CMake targets and public include layout.
- Make current library layers installable and consumable through CMake package exports.
- Add developer presets, quality checks, and cross-platform CI workflow files.
- Validate ECS entity, component, query, and system boundaries.
- Validate RHI resource, command, surface, swapchain, and submission contracts through the null backend.
- Prove host-owned surface embedding with a structured workspace example.

## Phase 2: Render Data Foundation — Complete

- Define `RenderBatch` and `InstanceLayout` as the universal renderer input contract, decoupling `truffle_render` from `truffle_ecs`.
- Establish `IFrameUploadRing` as the N-buffered CPU-to-GPU upload primitive at the RHI layer, supporting both ECS extraction and direct bulk upload.
- Introduce `truffle_scene` as the optional ECS-to-render bridge module.
- Define `IPipelineCache` interface for shader variant management.
- Expand ECS component definitions for cameras, lighting, meshes, and materials.

## Phase 3: Core Rendering Mechanics — Complete

- Material system support mapping shader parameters without hardcoding colors.
- Asynchronous fence dispatch routines across CPU and backends.
- Index draw primitives supporting `IndexFormat` and instanced indexed geometries.
- Embedded platform abstraction hooking into Metal view layers explicitly mapping native application shells to graphics.

## Phase 4: Production Backends — Complete

- Add memory pools reducing strict resource allocations at runtime.
- Add Indirect Draw buffers optimizing repeated dispatches avoiding CPU roundtrips.
- Support Shader Pipeline Variants executing hot-reload layouts using semantic layout hashes.
- Implement Metal for macOS.
- Implement Vulkan stubs bootstrapping initialization tests.
*(DirectX/OpenGL deferred to extended target builds as needed)*

## Phase 5: Compute & Frame Graph Orchestration — In Progress

- Implement GPU transform hierarchy compute pass for large node graphs processing local transform coordinate space arrays cleanly.
- Grow frame graph and render pass orchestration from the current submission baseline.
- Add shader reflection and binding layout direction without binding the public API to one GPU backend.
- Advance Vulkan capabilities to parity with the established Metal backend output.
