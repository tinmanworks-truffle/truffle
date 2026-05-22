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

Truffle is in Phase 2: Render Data Foundation. The renderer is now ECS-independent.
`RenderBatch` and `InstanceLayout` are the universal renderer input contract.
`IFrameUploadRing` provides N-buffered CPU-to-GPU upload memory. `truffle_scene`
is the new optional ECS extraction adapter. Three data lanes (ECS extraction,
bulk direct upload, future GPU-resident) are architecturally established.

## Current Work Status

- `truffle_render` has zero compile-time dependency on `truffle_ecs`. The
  `ExtractedFrame`/`SceneExtractor` API is replaced by `RenderBatch`/`InstanceLayout`.
- `IFrameUploadRing` is defined in `truffle_rhi`; `NullFrameUploadRing` implements
  it in the null backend. `IDevice::create_upload_ring()` is the factory.
- `truffle_scene` provides `SceneAdapter::extract(World&, IFrameUploadRing&)`
  returning a `SceneFrame` (camera state, light state, mesh batches). Transform
  data is written directly into ring memory — no per-entity intermediate copy.
- `IPipelineCache` interface and `NullPipelineCache` are in `truffle_render`.
- ADRs 0004-0006 lock the render-batch model, frame upload ring, and GPU
  transform hierarchy direction.
- 10 tests pass: 3 smoke, ECS contracts, RHI null, render flow, render batch,
  frame ring, scene adapter, package consumer. Package consumer exercises
  `Truffle::Scene`.
- `truffle_scene` is registered as an exported CMake target (`Truffle::Scene`).

## Relevant Decisions And Constraints

- Truffle is embeddable graphics infrastructure, not an application host or a
  dedicated game engine.
- Consumers may compose Truffle with FrameKit, but Truffle must not depend on it.
- Public rendering flow stays backend-neutral; production GPU backends are later
  roadmap work.
- `truffle_render` must have zero compile-time dependency on `truffle_ecs`.
  `truffle_scene` is the designated ECS-to-render bridge (see ADR 0004).
- `RenderBatch` and `InstanceLayout` are the universal renderer input contract.
  All three data lanes (ECS extraction, bulk direct, GPU-resident) converge on
  `RenderBatch`. `InstanceLayout::hash()` keys pipeline selection.
- `IFrameUploadRing` lives at the RHI layer. It is the N-buffered CPU-to-GPU
  upload primitive for both scene extraction and bulk streaming (see ADR 0005).
- `ChannelKind::LocalTransform` and `ParentIndex` are reserved for a future
  GPU-side compute pass for hierarchy resolution (see ADR 0006). No implementation
  yet.
- `IPipelineCache` is a placeholder interface; production shader variant selection
  is Phase 3/4 work.
- `truffle_scene` is optional. Consumers can build against `truffle_render` only
  and manage batches directly.
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
cmake --preset ci
cmake --build --preset ci
ctest --preset ci
cmake -S . -B build/foundation-default
cmake --build build/foundation-default
```

10 tests pass: 3 host workspace smoke checks, ECS contracts, null RHI, render
flow (direct batch), render batch types, frame ring, scene adapter (ECS→batch),
and installed package consumer. CI preset (warnings-as-errors) clean.
`grep -r "ecs/world" include/truffle/render/` returns nothing.

## Next Resume Steps

1. Read `AGENTS.md`, the README, contributor guidance, and the architecture
   docs before changing scope.
2. Check the current branch and open pull request state before starting work.
3. Phase 2 render data foundation branch is `feat/render-data-foundation`.
   Open a PR to `develop` when ready.
4. Phase 3 next steps: frame graph / render pass orchestration, shader
   reflection direction, material system boundary, GPU transform hierarchy
   compute pass.
5. Update this handoff before stopping on another machine.

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
