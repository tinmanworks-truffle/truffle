# Roadmap

## Phase 1: Contracts

- Establish modular CMake targets and public include layout.
- Validate ECS entity, component, query, and system boundaries.
- Validate RHI resource, command, surface, swapchain, and submission contracts
  through the null backend.
- Prove host-owned surface embedding and optional window-facing flows.

## Phase 2: Renderer Foundation

- Grow frame graph and render pass orchestration from the current frame
  extraction baseline.
- Add shader reflection and binding layout direction without binding the public
  API to one GPU backend.
- Expand render ECS components for cameras, lighting, meshes, and materials.

## Phase 3: Production Backends

- Implement Vulkan for Windows and Linux.
- Implement Direct3D for Windows.
- Implement Metal for macOS.
- Add OpenGL where compatibility and inspection workloads benefit from it.

