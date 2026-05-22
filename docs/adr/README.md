# Architecture Decision Records

Architecture decisions that need long-term tradeoff history belong here.

- `0001-layered-product-boundary.md` defines independently consumable Truffle
  layers and the host boundary.
- `0002-render-data-ingestion-lanes.md` keeps renderer-facing design open to ECS
  extraction and high-throughput data paths.
- `0003-backend-and-dependency-foundation.md` reserves backend ownership and
  dependency sourcing rules.
- `0004-render-batch-instance-layout.md` defines `RenderBatch` and
  `InstanceLayout` as the universal renderer input, decouples `truffle_render`
  from `truffle_ecs`, and establishes the three data-lane model.
- `0005-frame-upload-ring.md` establishes `IFrameUploadRing` as the
  backend-neutral N-buffered CPU-to-GPU upload primitive at the RHI layer.
- `0006-gpu-transform-hierarchy-direction.md` reserves `LocalTransform` and
  `ParentIndex` channel kinds and defers GPU compute-pass implementation to
  Phase 3/4.
