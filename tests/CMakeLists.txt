add_executable(truffle_ecs_tests ecs_contract_tests.cpp)
target_link_libraries(truffle_ecs_tests PRIVATE truffle_ecs)
add_test(NAME truffle_ecs_tests COMMAND truffle_ecs_tests)

add_executable(truffle_rhi_tests rhi_null_tests.cpp)
target_link_libraries(truffle_rhi_tests PRIVATE truffle_backend_null)
add_test(NAME truffle_rhi_tests COMMAND truffle_rhi_tests)

add_executable(truffle_render_tests render_flow_tests.cpp)
target_link_libraries(truffle_render_tests PRIVATE truffle_backend_null truffle_render)
add_test(NAME truffle_render_tests COMMAND truffle_render_tests)

if(TRUFFLE_INSTALL)
    add_test(
        NAME truffle_package_consumer
        COMMAND
            "${CMAKE_COMMAND}"
            "-DTRUFFLE_BINARY_DIR=${PROJECT_BINARY_DIR}"
            "-DTRUFFLE_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DTRUFFLE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/package-test/install"
            "-DTRUFFLE_CONSUMER_BUILD_DIR=${PROJECT_BINARY_DIR}/package-test/consumer"
            -P "${PROJECT_SOURCE_DIR}/cmake/VerifyInstall.cmake")
endif()
