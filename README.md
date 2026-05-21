# Truffle

Truffle is an embeddable C++ graphics engine and rendering system for tools,
simulations, CAD-style applications, games, and other graphics-heavy software.

## Status

- Stage: Draft
- Owner: TinMan
- License: Apache-2.0
- Visibility: Public
- Promotion criteria: Backend-neutral contracts, examples, and tests establish
  the first architecture baseline before production GPU backends land.

## What This Project Is

Truffle exists to give graphics-heavy applications a reusable ECS, renderer data
flow, and GPU abstraction without forcing them into a game engine or application
framework. It is not an application framework and it is not a dedicated game
engine.

The first implementation line focuses on stable architecture contracts before
production GPU backends:

- `truffle_core` owns shared status, configuration, and handle primitives.
- `truffle_ecs` provides a general-purpose ECS world.
- `truffle_rhi` defines backend-neutral GPU and presentation contracts.
- `truffle_backend_null` validates RHI flow without a GPU dependency.
- `truffle_render` extracts renderable ECS data and submits frames through RHI.
- `truffle_window` provides optional window and surface-facing contracts.

Truffle has no dependency on FrameKit. A consumer may compose both projects in
the same application when an application host and graphics system are useful
together.

## Build

```powershell
cmake -S . -B build -DTRUFFLE_BUILD_TESTS=ON -DTRUFFLE_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

See `docs/charter.md`, `docs/architecture.md`, and `docs/roadmap.md` for the
current project boundaries and backend direction.

## Attribution

If you use Truffle in your project, please mention Truffle and credit
TinMan / George Gil in your project documentation.

## Use

Embed only the targets your application needs. Host-owned window loops can link
the RHI and renderer targets directly; graphics-only experiments can also link
the optional window target and use its surface-facing helper contracts.

## Repository Layout

- `include/` public Truffle contracts
- `src/` target implementations
- `tests/` contract tests
- `examples/` embedding proofs
- `docs/` charter, architecture, and backend roadmap
