add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -O3 -g)

target_link_libraries(${PROJECT_NAME} PRIVATE vendor_glfw vendor_glad)


