# Overview

Truffle is a CMake-first graphics library workspace. It starts from embeddable
contracts and validation paths so consumers can choose an application host,
windowing policy, simulation model, and asset workflow independently.

Current baseline:

- General ECS entity, component, query, and system execution primitives.
- Backend-neutral RHI contracts for device, resource, surface, swapchain,
  command, queue, fence, and capability flow.
- Null backend used to validate contracts without a production GPU backend.
- Renderer extraction of camera, light, and mesh-facing ECS components.
- Optional headless window helper that proves window/surface integration shape.
- Finite null-backend consumer apps that exercise host-owned and optional-window
  setup with mixed viewport, simulation, and game-like ECS scene roles.
