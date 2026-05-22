add_library(truffle_scene STATIC scene_adapter.cpp)
target_link_libraries(truffle_scene PUBLIC truffle_ecs truffle_render)
truffle_configure_target(truffle_scene Scene)
add_library(Truffle::Scene ALIAS truffle_scene)
