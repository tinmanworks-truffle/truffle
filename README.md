# Truffle

Truffle is an embeddable C++ graphics engine and rendering system for tools,
simulations, CAD-style applications, games, and other graphics-heavy software.

## Status

- Stage: Draft
- Owner: TinMan
- License: Apache-2.0
- Visibility: Public
- Reason: Truffle is public engineering work and this repository contains the
  project baseline intended for outside review.
- Promotion criteria to Public: Backend-neutral contracts, examples, and tests
  establish the first architecture baseline before production GPU backends land.

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

Truffle has no dependency on FrameKit. A consumer may compose both projects in
the same application when an application host and graphics system are useful
together.

## Build

Prerequisites:

- CMake 3.23 or newer.
- A C++20-capable toolchain.
- A C compiler and the native window-system development dependencies needed by
  the copied GLFW source when examples are enabled.

```powershell
cmake -S . -B build -DTRUFFLE_BUILD_TESTS=ON -DTRUFFLE_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

See `docs/charter.md`, `docs/architecture.md`, and `docs/roadmap.md` for the
current project boundaries and backend direction.

## Continuity And AI Guidance

Cross-machine working state belongs in [`docs/handoff.md`](docs/handoff.md).
AI sessions should start with [`AGENTS.md`](AGENTS.md) and
[`AI_CONTEXT.md`](AI_CONTEXT.md). The local TinMan Doctrine snapshot lives under
[`docs/doctrine/`](docs/doctrine/README.md).

## Attribution

If you use Truffle in your project, please mention Truffle and credit
TinMan / George Gil in your project documentation.

## Use

Embed only the targets your application needs. Host-owned window loops can link
the RHI and renderer targets directly and provide their own native surface
boundary.

## Repository Layout

- `include/` public Truffle contracts
- `src/` target implementations
- `tests/` contract tests
- `examples/` host-owned workspace integration proof
- `docs/` charter, architecture, backend roadmap, handoff, and doctrine snapshot

## Contributing

See `CONTRIBUTING.md` for the branch workflow, commit format, signing
expectations, pull request path into protected `develop`, and stable promotion
path into `master`.
