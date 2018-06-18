include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_executable(p4lsd main.cpp)
target_link_libraries(p4lsd PRIVATE p4ls)
