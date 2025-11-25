get_filename_component(CUR_DIR "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

file(GLOB ALL_CPP_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(ALL_CPP_FILES "")
foreach(CPP_PATH ${ALL_CPP_FILES_PATH}) 
  get_filename_component(CPP_FILE "${CPP_PATH}" NAME)
  list(APPEND ALL_CPP_FILES "${CPP_FILE}")
endforeach()

add_executable(${CUR_DIR})
target_sources(${CUR_DIR} PRIVATE ${ALL_CPP_FILES})
target_compile_options(${CUR_DIR} PRIVATE -Wall -O3 -g)
target_link_libraries(${CUR_DIR} PRIVATE vendor_glfw vendor_glad stb_image vendor_glm)
set_target_properties(
  ${CUR_DIR} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CUR_DIR}
)

file(
  COPY assets/
  DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CUR_DIR}/assets
)
