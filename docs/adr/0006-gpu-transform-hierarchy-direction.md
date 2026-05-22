# ADR 0006: GPU Transform Hierarchy Direction

## Status

Accepted — implementation deferred to Phase 3/4.

## Context

Scene graphs with parent-child transform chains require computing world-space
matrices before rendering. The naive approach resolves the hierarchy sequentially
on the CPU each frame: traverse from root to leaves, multiply local transforms
into parent world matrices. For large hierarchies this is O(depth × node count)
and is difficult to parallelize without a careful thread-split over independent
subtrees.

In a previous engineering visualization tool with dense sensor detection data,
moving transform-hierarchy resolution to a GPU compute pass produced measurable
performance improvements by exploiting data parallelism across nodes that share
no dependencies at the same hierarchy depth.

## Decision

Truffle's transform model is designed to allow GPU-side world-matrix resolution:

- `ChannelKind::LocalTransform` carries a local-space matrix per node.
- `ChannelKind::ParentIndex` carries a signed 32-bit parent node index per node
  (`-1` for roots).
- A future compute pass reads `LocalTransform[]` + `ParentIndex[]` and writes
  `Transform[]` (world-space matrices) for use by draw passes.

`Transform` in `components.hpp` continues to carry a world-space matrix as the
current convenient path. Phase 2 does not break or replace that path.

The `InstanceLayout` channel kinds reserve `LocalTransform` and `ParentIndex`
now so future bulk-upload callers can declare them without an API change.

## Consequences

- No GPU compute pass is added in Phase 2.
- `ChannelKind::LocalTransform` and `ChannelKind::ParentIndex` are reserved in
  the enum with no production user until Phase 3/4.
- Future bulk ingestion of sensor data with relative coordinate frames can use
  the GPU hierarchy path without per-entity CPU chain traversal.
- API surface for the compute pass (dispatch size, buffer bindings, output
  buffer slot) is design work for the Phase 3/4 milestone that implements it.
