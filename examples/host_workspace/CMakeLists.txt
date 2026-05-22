set(GLFW_BUILD_DOCS OFF CACHE BOOL "Do not build vendored GLFW docs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Do not build vendored GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Do not build vendored GLFW tests" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Do not install vendored GLFW" FORCE)

add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)

add_executable(
    truffle_host_workspace_example
    src/application.cpp
    src/cad_workspace.cpp
    src/cli.cpp
    src/editor_workspace.cpp
    src/host_window.cpp
    src/main.cpp
    src/simulation_workspace.cpp
    src/workspace.cpp)
target_include_directories(truffle_host_workspace_example PRIVATE src)
target_link_libraries(
    truffle_host_workspace_example
    PRIVATE glfw truffle_backend_null truffle_render)

if(TRUFFLE_BUILD_TESTS)
    add_test(
        NAME truffle_host_workspace_editor_smoke
        COMMAND truffle_host_workspace_example --smoke --workspace editor)
    add_test(
        NAME truffle_host_workspace_cad_smoke
        COMMAND truffle_host_workspace_example --smoke --workspace cad)
    add_test(
        NAME truffle_host_workspace_simulation_smoke
        COMMAND truffle_host_workspace_example --smoke --workspace simulation)
endif()
