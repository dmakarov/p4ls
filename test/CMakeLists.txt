enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

hunter_add_package(Boost COMPONENTS test)
find_package(Boost CONFIG REQUIRED COMPONENTS unit_test_framework)

add_executable(unittests_driver
  lsp_server_test.cpp
  unittests_driver.cpp)
target_include_directories(unittests_driver PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(unittests_driver PUBLIC p4ls Boost::unit_test_framework)

add_test(NAME unittests COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittests_driver)
# WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
