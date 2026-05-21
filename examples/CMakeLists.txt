add_executable(truffle_host_surface_example host_surface_example.cpp)
target_link_libraries(
    truffle_host_surface_example
    PRIVATE truffle_backend_null truffle_render)
add_executable(truffle_window_example window_example.cpp)
target_link_libraries(
    truffle_window_example
    PRIVATE truffle_backend_null truffle_render truffle_window)

if(TRUFFLE_BUILD_TESTS)
    add_test(
        NAME truffle_host_surface_example_smoke
        COMMAND truffle_host_surface_example)
    add_test(
        NAME truffle_window_example_smoke
        COMMAND truffle_window_example)
endif()
