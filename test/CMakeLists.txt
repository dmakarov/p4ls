enable_testing()

hunter_add_package(Boost COMPONENTS test)
find_package(Boost CONFIG REQUIRED COMPONENTS unit_test_framework)

add_executable(unittests_driver
  lsp_server_test.cpp)
target_link_libraries(unittests_driver PUBLIC p4ls Boost::unit_test_framework)

add_test(NAME unittests COMMAND unittests_driver)
