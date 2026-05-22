# Handoff

Use this document as Truffle's rolling cross-machine handoff. Keep it useful to
a human resuming work and to a fresh AI session reading the repository.

Do not store raw AI transcripts, secrets, personal-only notes, or machine-private
details here. Promote durable project direction into the roadmap, architecture
docs, charter, or ADRs instead of leaving it only in this working document.

## Current Focus

Truffle is establishing its first architecture baseline for an embeddable C++20
graphics and rendering system. Current code proves ECS, renderer extraction,
backend-neutral RHI flow, the null backend, host-owned surfaces, and the optional
window-facing integration path before production GPU backends land.

## Current Work Status

- The repository is still in the Phase 1 contract stage described by the
  roadmap.
- Public targets exist for core, ECS, RHI, the null backend, rendering, and the
  optional window layer.
- Finite null-backend examples exercise both a host-owned surface consumer and a
  consumer using Truffle's headless window helper.
- Tests cover ECS contract flow, null RHI flow, renderer flow, and smoke checks
  for both examples.
- Repository continuity now uses this tracked handoff plus the local Doctrine AI
  baseline rooted at `AGENTS.md`.

## Relevant Decisions And Constraints

- Truffle is embeddable graphics infrastructure, not an application host or a
  dedicated game engine.
- Consumers may compose Truffle with FrameKit, but Truffle must not depend on it.
- Public rendering flow stays backend-neutral; production GPU backends are later
  roadmap work.
- Keep active handoff state curated and public-safe. Lasting decisions belong in
  stable docs or ADRs.
- Normal feature and fix work targets protected `develop`; stable promotion goes
  through `master`.

## Last Verified Commands And Checks

Verified on 2026-05-22:

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

The configured suite passed with five tests: two example smoke checks and the
ECS, null RHI, and render flow tests.

## Next Resume Steps

1. Read `AGENTS.md`, the README, contributor guidance, and the architecture
   docs before changing scope.
2. Check the current branch and open pull request state before starting the next
   contract change.
3. Continue Phase 1 contract work or refine the example branch based on the next
   selected issue or review target.
4. Update this handoff before stopping on another machine.

## Open Questions Or Risks

- Decide when Truffle should add tracked CI, PR-title checks, and required status
  checks as the public baseline grows. They are intentionally outside the
  continuity and docs alignment change.
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
