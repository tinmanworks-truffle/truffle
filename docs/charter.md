# Truffle Charter

## Purpose

Truffle is a generalized graphics engine and rendering system. It gives C++
applications layered rendering modules and GPU abstraction without requiring a
game-engine object model or application framework.

## In Scope

- Backend-neutral RHI and renderer contracts.
- Render data paths that can support extracted scenes and future high-throughput
  bulk or GPU-oriented workflows.
- Render-facing ECS components, scene extraction, and a general ECS suitable for
  tools, simulations, and games when a consumer wants that integration.
- Framework-facing rendering modules that build above lower Truffle layers
  without making them mandatory.
- Presentation contracts for consumer-owned native surfaces.
- Validation backends, examples, and contract tests that keep backend work honest.

## Out of Scope

- Application-host ownership, input policy, and application lifecycle policy.
- A game editor, gameplay framework, or game-specific asset model.
- A required dependency on FrameKit or another host framework.
- A requirement that consumers adopt Truffle ECS before using the renderer.
- Backend-specific public API leakage outside explicit capabilities or extensions.

## Integration Boundary

Truffle can run inside a consumer-owned loop and on a consumer-owned native
surface. FrameKit and Truffle remain independent projects; applications that
need both compose them at their own boundary.
