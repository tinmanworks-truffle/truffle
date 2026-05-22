# Handoff

Use this document as Truffle's rolling cross-machine handoff. Keep it useful to
a human resuming work and to a fresh AI session reading the repository.

AI work that reaches a commit must update this handoff before that commit so
status, decisions, verification, next steps, and open risks remain current as
applicable.

Do not store raw AI transcripts, secrets, personal-only notes, or machine-private
details here. Promote durable project direction into the roadmap, architecture
docs, charter, or ADRs instead of leaving it only in this working document.

## Current Focus

Truffle is establishing its first architecture and build baseline for an
embeddable C++20 layered graphics and rendering system. Current work keeps RHI,
renderer, ECS integration, and future framework-facing layers independently
consumable before production GPU backends land.

## Current Work Status

- The repository is still in the Phase 1 contract stage described by the
  roadmap.
- Public targets exist for core, ECS, RHI, the null backend, and rendering, and
  the foundation pass is making them installable CMake package exports.
- Backend module folders and reserved CMake options now establish ownership for
  future Vulkan, Direct3D, Metal, and OpenGL work without implementing them yet.
- The host workspace example owns a visible GLFW window and separates editor,
  CAD, and simulation-facing workspace roles over Truffle's null-backed path.
- Tests cover ECS contract flow, null RHI flow, renderer flow, package-consumer
  install/export flow, and smoke checks for each host workspace role.
- Repository continuity now uses this tracked handoff plus the local Doctrine AI
  baseline rooted at `AGENTS.md`, including the rule that AI-authored commits
  keep this handoff current.

## Relevant Decisions And Constraints

- Truffle is embeddable graphics infrastructure, not an application host or a
  dedicated game engine.
- Consumers may compose Truffle with FrameKit, but Truffle must not depend on it.
- Public rendering flow stays backend-neutral; production GPU backends are later
  roadmap work.
- Renderer-facing design is data-oriented: ECS extraction remains a first-party
  lane, while dense workloads need future bulk or GPU-oriented ingestion lanes.
- Truffle does not own native window helpers in the current baseline. Host apps
  provide their own windowing and native surface boundary.
- Runtime dependencies should prefer Git submodules when practical; narrow
  pinned source copies remain the fallback when a submodule does not fit.
- The host workspace keeps a pinned, example-local GLFW source copy narrowed to
  the GLFW library build path rather than a submodule or full upstream repo
  snapshot.
- Keep active handoff state curated and public-safe. Lasting decisions belong in
  stable docs or ADRs.
- Normal feature and fix work targets protected `develop`; stable promotion goes
  through `master`.

## Last Verified Commands And Checks

Verified on 2026-05-22:

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
cmake --build --preset dev --target truffle_format_check
cmake --preset ci
cmake --build --preset ci
ctest --preset ci
cmake -S . -B build/foundation-default
cmake --build build/foundation-default
ctest --test-dir build/foundation-default --output-on-failure
```

The configured suite passed with seven tests: three host workspace smoke checks,
the ECS, null RHI, and render flow tests, and the installed package consumer
check. Both preset and default top-level paths passed. The format-check target
is available but only prints an install hint on this machine because
`clang-format` is not currently found.

## Next Resume Steps

1. Read `AGENTS.md`, the README, contributor guidance, and the architecture
   docs before changing scope.
2. Check the current branch and open pull request state before starting the next
   contract change.
3. Review the foundation branch architecture, CMake exports, and workflow files
   before moving to render-data API design.
4. Update this handoff before stopping on another machine.

## Open Questions Or Risks

- Workflow files now establish cross-platform CI names, but branch rules,
  required status checks, and PR-title checks still need a follow-up governance
  pass after the workflow path proves reliable.
- Truffle is not currently GitHub Project-managed. If that changes, Doctrine's
  issue-driven execution guidance becomes mandatory for planned work.

## Curated Ideas Parking Lot

- Use this short list for project-relevant ideas that need another pass before
  they become roadmap items, ADRs, or implementation work.
- Keep new entries specific enough that a later session can decide whether to
  promote or discard them.

## Promotion Rule

Move stable architecture, scope, roadmap, governance, or tradeoff decisions out
of this handoff when they become durable project truth. Use the charter,
architecture docs, roadmap, contributor guidance, or an ADR as appropriate.
