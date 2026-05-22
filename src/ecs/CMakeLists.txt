add_library(truffle_ecs STATIC world.cpp)
target_link_libraries(truffle_ecs PUBLIC truffle_core)
truffle_configure_target(truffle_ecs ECS)
add_library(Truffle::ECS ALIAS truffle_ecs)
