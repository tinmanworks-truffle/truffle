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

Truffle is in Phase 3B: Metal backend. A fully functional Metal backend is on
branch `feat/phase3-metal-backend`, pending PR to `develop`. Phase 3C
(Material system) is next.

## Current Work Status

- Phase 3A + 3B complete on `feat/phase3-metal-backend`. 11/11 tests pass; CI
  preset warnings-as-errors clean.
- `include/truffle/rhi/rhi.hpp`: added `ISwapchain::schedule_present(ICommandBuffer&)`
  to let swapchain implementations hook presentation into the command buffer
  before commit. `ICommandBuffer` forward-declared to avoid circular ordering.
- `include/truffle/rhi/metal_backend.hpp` + `src/backends/metal/metal_backend.mm`:
  full Metal backend implementation:
  - `MetalBuffer`: `MTLResourceStorageModeShared` (CPU+GPU visible, Phase 3B).
  - `MetalTexture`: `MTLStorageModePrivate` + `RenderTarget|ShaderRead` usage.
  - `MetalSampler`, `MetalShader` (MSL source compiled via
    `newLibraryWithSource:options:error:`), `MetalPipeline`
    (`MTLRenderPipelineState`, BGRA8Unorm colour attachment, Phase 3B).
  - `MetalSurface` (wraps `CAMetalLayer*` from `cocoa_layer` native handle);
    `MetalSwapchain` (headless offscreen or CAMetalLayer-backed).
  - `MetalCommandBuffer`: full render pass + resource binding + draw calls via
    `id<MTLRenderCommandEncoder>`.
  - `MetalQueue::submit`: calls `[cmdBuf commit]`; blocking
    `waitUntilCompleted` when a fence is requested (Phase 3B; async in 3C).
  - `MetalFrameUploadRing`: N `MTLResourceStorageModeShared` buffers.
  - `MetalDevice`, `MetalBackend` (`MTLCreateSystemDefaultDevice`).
- `cmake/TruffleOptions.cmake`: Metal option defaults ON for Apple top-level builds.
- `src/backends/CMakeLists.txt`: Metal backend wired in (Apple-only guard).
- `tests/metal_backend_tests.cpp`: exercises buffer, shader compile, pipeline,
  headless swapchain, full render pass, upload ring, fence, resize.
  Skips gracefully when no Metal GPU is present.

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

Verified on 2025-05-22 (Phase 3B):

```sh
cmake --preset dev  -DTRUFFLE_BUILD_BACKEND_METAL=ON
cmake --build --preset dev
ctest --preset dev   # 11/11
cmake --preset ci   -DTRUFFLE_BUILD_BACKEND_METAL=ON
cmake --build --preset ci
ctest --preset ci    # 11/11
```

11 tests: 3 host workspace smoke, ECS, null RHI, render flow, render batch,
frame ring, scene adapter, Metal backend, package consumer.

## Next Resume Steps

1. Read `AGENTS.md`, the README, contributor guidance, and architecture docs.
2. Check current branch (`feat/phase3-metal-backend`) and open PR state.
3. Open a PR from `feat/phase3-metal-backend` → `develop`, get review.
4. Phase 3C: Material system.
   - Add `colorFormat` field to `PipelineDesc` (remove the BGRA8Unorm hardcode).
   - Real `IPipelineCache` keyed on `InstanceLayout::hash()` + material.
   - Uniform buffer upload path via `IFrameUploadRing`.
   - `IShader` bytecode loading utility (file or embedded bytes).
5. Phase 3D: Async fence (replace blocking `waitUntilCompleted` with
   `dispatch_semaphore` completion handler in Metal backend).
6. Update this handoff before stopping on another machine.

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
