add_executable(truffle_host_surface_example host_surface_example.cpp)
target_link_libraries(
    truffle_host_surface_example
    PRIVATE truffle_backend_null truffle_render)

add_executable(truffle_window_example window_example.cpp)
target_link_libraries(
    truffle_window_example
    PRIVATE truffle_backend_null truffle_render truffle_window)

