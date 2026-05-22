# Host Workspace Vendor Sources

This example keeps example-only dependencies inside its own folder so they do
not become Truffle runtime dependencies.

## GLFW

- Source: GLFW 3.4 tag
- Upstream commit: `7b6aead9fb88b3623e3b3725ebb42670cbe4c579`
- Location: `glfw/`

The GLFW source copy retains its upstream license and build files. The host
workspace CMake target disables GLFW docs, tests, examples, and install targets
because this vendored copy only supplies the window host for this example.
