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

Truffle is in Phase 3A: RHI Contract Enrichment. The render-pass model, resource
binding, and shader-stage contracts have been landed on branch
`feat/phase3-rhi-contracts`. Phase 3B (Metal backend) is next.

## Current Work Status

- Phase 3A complete on `feat/phase3-rhi-contracts`. 10/10 tests pass; CI preset
  warnings-as-errors clean.
- `include/truffle/rhi/rhi.hpp` enriched:
  - `ShaderStage`, `PrimitiveTopology`, `LoadOp`, `StoreOp` enums.
  - `ShaderDesc` carries a `stage` field.
  - `PipelineDesc` carries `vertexShader`, `fragmentShader`, `topology`,
    `depthWrite` alongside the existing `depthTest` and `debugName`.
  - `RenderPassDesc`, `ColorAttachmentDesc`, `DepthAttachmentDesc`, `ClearColor`
    structs define inline render pass configuration.
  - `ISwapchain::acquire_next_texture()` returns the current frame drawable.
  - `ICommandBuffer` expanded: `begin_render_pass`, `end_render_pass`,
    `bind_pipeline`, `bind_vertex_buffer`, `bind_index_buffer`, `set_viewport`,
    `set_scissor`; draw methods rearranged into logical sections.
- `NullSwapchain::acquire_next_texture()` lazily allocates a `NullTexture`
  drawable and returns it each frame.
- `NullCommandBuffer` has stub implementations for all new methods; each
  guards on `State::recording`.
- `Renderer::render()` now takes an optional `rhi::ISwapchain*` (default
  `nullptr`). When supplied it acquires the next texture and builds a proper
  `RenderPassDesc`; headless path uses a `{1,1}` placeholder extent.
  The render loop calls `bind_pipeline`, `bind_vertex_buffer`, `begin/end_render_pass`.
- Tests updated: `rhi_null_tests` exercises full enriched command sequence;
  `render_flow_tests` covers both headless and swapchain render paths.
- ADR 0007 records the inline render-pass model decision.

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

Verified on 2025-05-22 (Phase 3A):

```sh
cmake --build --preset dev
ctest --preset dev
cmake --build --preset ci
ctest --preset ci
```

10 tests pass (dev + ci). CI preset warnings-as-errors clean.
All new ICommandBuffer stubs, swapchain acquire, and render-pass flow exercised.

## Next Resume Steps

1. Read `AGENTS.md`, the README, contributor guidance, and architecture docs.
2. Check current branch (`feat/phase3-rhi-contracts`) and open PR state.
3. Open a PR from `feat/phase3-rhi-contracts` → `develop`, get review.
4. Phase 3B: Metal backend in `src/backends/metal/`.
   - `NullBackend` as structural guide; implement `MetalDevice`, `MetalCommandBuffer`,
     `MetalSwapchain`, `MTLRenderPassDescriptor` mapping from `RenderPassDesc`.
   - Add `TRUFFLE_BACKEND_METAL` CMake option.
5. Phase 3C: Material system — uniform upload path, `IShader` bytecode loading,
   real `IPipelineCache` with layout+material hash keying.
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
