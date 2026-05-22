if(TRUFFLE_BUILD_BACKEND_NULL)
    add_subdirectory(null)
endif()

function(truffle_defer_backend option backend)
    if(${option})
        message(FATAL_ERROR
            "${backend} is reserved by ${option}, but its Truffle backend "
            "implementation has not landed yet.")
    endif()
endfunction()

truffle_defer_backend(TRUFFLE_BUILD_BACKEND_VULKAN "Vulkan")
truffle_defer_backend(TRUFFLE_BUILD_BACKEND_DIRECT3D "Direct3D")
truffle_defer_backend(TRUFFLE_BUILD_BACKEND_METAL "Metal")
truffle_defer_backend(TRUFFLE_BUILD_BACKEND_OPENGL "OpenGL")
