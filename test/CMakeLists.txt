enable_testing()

if (HUNTER_ENABLED)
    hunter_add_package(Boost COMPONENTS test)
    find_package(Boost CONFIG REQUIRED COMPONENTS unit_test_framework)
else()
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
endif()

include_directories(../vendor/p4c ../vendor/p4c/test/frameworks/gtest/googletest/include ${CMAKE_BINARY_DIR}/p4c)

add_executable(unittests_driver
  lexer_test.cpp
  lsp_server_test.cpp
  protocol_test.cpp
  unittests_driver.cpp)
target_include_directories(unittests_driver PUBLIC ${PROJECT_SOURCE_DIR}/src/lsp ${PROJECT_SOURCE_DIR}/src/p4l)
target_link_libraries(unittests_driver
  lsp
  p4l
  p4ctoolkit
  frontend
  ir
  Boost::boost
  Boost::log
  Boost::regex
  Boost::filesystem
  Boost::iostreams
  Boost::date_time
  Boost::thread
  Boost::system
  Boost::unit_test_framework
  ${LIBGMP_LIBRARIES})

add_test(NAME unittests COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittests_driver)
