# Architecture

## Layers

1. `truffle_core` provides shared status, configuration, and identity primitives.
2. `truffle_ecs` provides a general ECS world independent of rendering policy.
3. `truffle_rhi` defines devices, queues, commands, resources, pipelines,
   surfaces, swapchains, synchronization, and capability descriptions.
4. `truffle_render` extracts ECS scene data and builds frame submissions above
   the RHI.
5. `truffle_window` adds optional window and native-surface facing contracts.
6. `truffle_backend_*` targets implement RHI contracts behind the public layer.

## Repository Shape

Public contracts stay under `include/truffle`. Runtime implementations stay
under `src`. The first workspace uses finite consumer examples and tests as
proof of the host-owned surface path, the optional window-facing path, ECS
queries, resource setup, swapchain resize, and RHI submission flow.

## Data Flow

Rendering reads graphics-facing components from an ECS world, creates an
extracted frame, records backend-neutral commands, and submits them through an
RHI queue. Production backends can translate that sequence to Vulkan, Direct3D,
OpenGL, or Metal without changing higher-level ECS ownership.

## Current Baseline

The repository starts with contracts, a null backend, examples, and tests. The
null backend validates resource creation, command recording, surface/swapchain
flow, and submission flow without requiring a GPU or native window. The richer
example apps keep application loops outside Truffle while simulating mixed
viewport, simulation, and game-like render usage.
