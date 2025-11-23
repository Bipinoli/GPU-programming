add_subdirectory(glfw)

add_library(vendor_glfw INTERFACE)
target_link_libraries(vendor_glfw INTERFACE glfw)
# GLFW_INCLUDE_NONE asks GLFW to not include OpenGL related functions
# As we get them from glad
target_compile_definitions(vendor_glfw INTERFACE GLFW_INCLUDE_NONE)

add_library(vendor_glad SHARED glad/src/glad.c)
target_include_directories(vendor_glad PUBLIC glad/include)
set_target_properties(
  vendor_glad
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/vendor/glad"
)

add_library(stb_image SHARED stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC stb_image/)
set_target_properties(
  stb_image
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/vendor/stb_image"
)

