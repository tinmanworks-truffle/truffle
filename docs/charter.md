# Truffle Charter

## Purpose

Truffle is a generalized graphics engine and rendering system. It gives C++
applications an embeddable ECS, renderer data flow, and GPU abstraction without
requiring a game-engine object model or application framework.

## In Scope

- Backend-neutral RHI and renderer contracts.
- Render-facing ECS components and scene extraction.
- A general ECS suitable for tools, simulations, and games.
- Presentation contracts for consumer-owned native surfaces.
- Validation backends, examples, and contract tests that keep backend work honest.

## Out of Scope

- Application-host ownership, input policy, and application lifecycle policy.
- A game editor, gameplay framework, or game-specific asset model.
- A required dependency on FrameKit or another host framework.
- Backend-specific public API leakage outside explicit capabilities or extensions.

## Integration Boundary

Truffle can run inside a consumer-owned loop and on a consumer-owned native
surface. FrameKit and Truffle remain independent projects; applications that
need both compose them at their own boundary.
