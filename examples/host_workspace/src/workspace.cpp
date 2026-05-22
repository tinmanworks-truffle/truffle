#include "workspace.hpp"

namespace truffle::examples::host_workspace {

[[nodiscard]] std::unique_ptr<IWorkspace> make_editor_workspace();
[[nodiscard]] std::unique_ptr<IWorkspace> make_cad_workspace();
[[nodiscard]] std::unique_ptr<IWorkspace> make_simulation_workspace();

std::optional<WorkspaceKind> parse_workspace_kind(std::string_view name) {
    if (name == "editor") {
        return WorkspaceKind::editor;
    }
    if (name == "cad") {
        return WorkspaceKind::cad;
    }
    if (name == "simulation") {
        return WorkspaceKind::simulation;
    }
    return std::nullopt;
}

std::string_view workspace_name(WorkspaceKind kind) noexcept {
    switch (kind) {
    case WorkspaceKind::editor:
        return "editor";
    case WorkspaceKind::cad:
        return "cad";
    case WorkspaceKind::simulation:
        return "simulation";
    }
    return "unknown";
}

std::unique_ptr<IWorkspace> create_workspace(WorkspaceKind kind) {
    switch (kind) {
    case WorkspaceKind::editor:
        return make_editor_workspace();
    case WorkspaceKind::cad:
        return make_cad_workspace();
    case WorkspaceKind::simulation:
        return make_simulation_workspace();
    }
    return {};
}

} // namespace truffle::examples::host_workspace
