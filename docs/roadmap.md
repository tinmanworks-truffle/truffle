# Roadmap

## Phase 1: Foundation And Contracts

- Establish modular CMake targets and public include layout.
- Make current library layers installable and consumable through CMake package
  exports.
- Add developer presets, quality checks, and cross-platform CI workflow files.
- Validate ECS entity, component, query, and system boundaries.
- Validate RHI resource, command, surface, swapchain, and submission contracts
  through the null backend.
- Prove host-owned surface embedding with a structured workspace example.

## Phase 2: Render Data Foundation

- Define renderer-facing data interfaces beyond the current ECS extraction path.
- Reserve bulk and GPU-oriented data-flow direction for dense dynamic datasets
  before locking higher renderer APIs around per-entity extraction.
- Grow frame graph and render pass orchestration from the current submission
  baseline.
- Add shader reflection and binding layout direction without binding the public
  API to one GPU backend.
- Expand render ECS adapters for cameras, lighting, meshes, and materials.

## Phase 3: Rendering Framework Layers

- Add higher rendering layers for materials, asset-facing boundaries, debug
  hooks, and tool-facing orchestration without hiding low-level modules.
- Keep scene adapters optional so consumers can choose Truffle ECS or their own
  data source.

## Phase 4: Production Backends

- Implement Vulkan for Windows and Linux.
- Implement Direct3D for Windows.
- Implement Metal for macOS.
- Add OpenGL where compatibility and inspection workloads benefit from it.
