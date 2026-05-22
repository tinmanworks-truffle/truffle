# Overview

Truffle is a CMake-first layered graphics library workspace. It starts from
embeddable contracts, independently linkable modules, and validation paths so
consumers can choose an application host, windowing policy, simulation model,
and asset workflow independently.

Current baseline:

- General ECS entity, component, query, and system execution primitives.
- Backend-neutral RHI contracts for device, resource, surface, swapchain,
  command, queue, fence, and capability flow.
- Null backend used to validate contracts without a production GPU backend.
- Renderer flow that currently proves ECS extraction while reserving non-ECS and
  high-throughput render-data lanes for later rendering work.
- Host-owned workspace example that uses a visible GLFW window for editor, CAD,
  and simulation-facing app roles while the Truffle path stays null-backed.
- Installable CMake targets plus presets and CI paths for the current foundation.
