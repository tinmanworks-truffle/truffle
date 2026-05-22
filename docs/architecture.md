# Architecture

## Layers

Truffle is one product with modules that stay linkable at different levels:

1. Foundation modules such as `truffle_core` provide shared status,
   configuration, and identity primitives.
2. `truffle_rhi` defines backend-neutral devices, queues, commands, resources,
   pipelines, surfaces, swapchains, synchronization, and capabilities.
3. `truffle_backend_*` modules implement RHI contracts behind backend ownership.
   The null backend is buildable now; Vulkan, Direct3D, Metal, and OpenGL have
   reserved module and build-option boundaries for later milestones.
4. Rendering modules such as `truffle_render` build frame submission and future
   frame graph, shader, material, and orchestration direction above RHI.
5. Scene integration modules provide first-party adapters such as Truffle ECS
   components and extraction helpers without making ECS the only renderer input.
6. Future framework-facing modules can add assets, debug hooks, and tool-facing
   rendering workflows above the lower layers without hiding them from consumers.

## Repository Shape

Public contracts stay under `include/truffle`. Runtime implementations stay
under module-owned `src` directories, with backend code under `src/backends`.
CMake helpers under `cmake` own options, warnings, formatting hooks, install
rules, and package export behavior. Examples and tests remain consumer proof and
validation layers instead of runtime dependencies.

## Data Flow

Renderer-facing flow is data-oriented. The current bootstrap reads
graphics-facing components from an ECS world, creates an extracted frame, records
backend-neutral commands, and submits them through an RHI queue. This is the
convenient scene integration lane.

Large dynamic workloads need a second lane. LiDAR, radar, dense simulation, and
similar datasets must be able to move toward bulk render data, streaming buffers,
or GPU-resident generation without a mandatory loop that translates millions of
high-level entities into renderer-owned objects each frame. Later renderer APIs
must preserve that option while using the same RHI and backend contracts.

Production backends can translate those backend-neutral submissions to Vulkan,
Direct3D, OpenGL, or Metal without changing consumer host ownership.

## Host Boundary

Truffle does not own native windowing, input policy, application lifetime, or
the consumer simulation model. Consumers provide host loops and native surface
boundaries, then choose whether to link low-level RHI modules, renderer modules,
or future higher-level framework modules.

## Dependency Boundary

Runtime dependencies prefer Git submodules when that is practical. Pinned
source copies are the fallback when submodules do not fit, and they must retain
license, provenance, and a narrow build footprint. Example-only dependencies
stay inside their example folders so they do not become Truffle runtime
dependencies.

## Current Baseline

The repository starts with contracts, a null backend, packageable CMake targets,
examples, and tests. The null backend validates resource creation, command
recording, surface/swapchain flow, and submission flow without requiring a GPU.
The host workspace example keeps native window ownership and application loops
outside Truffle while separating editor, CAD, and simulation scene roles.
