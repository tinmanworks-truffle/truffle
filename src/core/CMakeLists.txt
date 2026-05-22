add_library(truffle_core STATIC core.cpp)
truffle_configure_target(truffle_core Core)
add_library(Truffle::Core ALIAS truffle_core)
