add_subdirectory(glfw)

add_library(vendor_glfw INTERFACE)
target_link_libraries(vendor_glfw INTERFACE glfw)
target_compile_definitions(vendor_glfw INTERFACE GLFW_INCLUDE_NONE)

add_library(vendor_glad STATIC glad/src/glad.c)
target_include_directories(vendor_glad PUBLIC glad/include)

