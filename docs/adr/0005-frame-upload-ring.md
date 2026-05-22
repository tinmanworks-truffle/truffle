# ADR 0005: Frame Upload Ring

## Status

Accepted

## Context

Moving per-frame CPU data to the GPU requires managing upload memory that is
safe to write while prior frames may still be in flight. Without a structured
approach, callers either stall waiting for prior frames to complete or risk
writing into memory that the GPU is actively reading.

Phase 1 had no upload-ring primitive. The null backend had no concept of
mapped GPU memory. Phase 2 introduces bulk upload (Lane B in ADR 0004), which
requires a concrete mechanism for callers to obtain CPU-writable GPU-visible
memory and for the system to reclaim it after the GPU finishes with it.

## Decision

`IFrameUploadRing` is a backend-neutral N-buffered upload memory manager living
at the RHI layer. `IDevice::create_upload_ring(frames_in_flight, capacity)` is
the factory.

Each call to `allocate(size, alignment)` returns a `FrameAllocation` containing:

- A pointer to the backing `IBuffer`.
- A byte offset into that buffer.
- A CPU-mapped pointer callers write directly into.
- The allocated size.

`advance()` reclaims the oldest completed frame's range. Callers must call
`advance()` once per frame after the corresponding GPU work is known complete
(guarded by a fence in real backends; unconditional in the null backend).

The null backend implementation backs the ring with a plain `std::vector<byte>`
and returns valid CPU pointers so tests can write and read data without a GPU.

## Consequences

- Lane A (ECS extraction) and Lane B (bulk upload) both write through
  `IFrameUploadRing`, making upload memory management uniform.
- Real GPU backends implement `IFrameUploadRing` over persistent-mapped
  host-visible memory with fence-gated reclaim.
- `FrameAllocation::buffer` is a valid `IBuffer*` in all backends, so callers
  can pass it directly to draw-call binding without special-casing.
- A zero `FrameAllocation` (null buffer pointer) signals ring exhaustion;
  callers must check before writing.
- Ring capacity tuning is a caller responsibility; the interface exposes
  `frames_in_flight()` and `capacity_per_frame()` for introspection.
