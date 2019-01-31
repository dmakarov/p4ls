enable_testing()

if (HUNTER_ENABLED)
    hunter_add_package(Boost COMPONENTS test)
    find_package(Boost CONFIG REQUIRED COMPONENTS unit_test_framework)
else()
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
endif()

add_executable(unittests_driver
  lexer_test.cpp
  lsp_server_test.cpp
  protocol_test.cpp
  wave_test.cpp
  unittests_driver.cpp)

target_compile_options(unittests_driver PRIVATE "-g" "-Wall" "-Werror" "-Wextra" "-fvisibility=hidden" "-fvisibility-inlines-hidden")

target_include_directories(unittests_driver
  PUBLIC
  ${PROJECT_SOURCE_DIR}/server/lsp
  ${PROJECT_SOURCE_DIR}/server/p4l)

target_link_libraries(unittests_driver
  lsp
  p4l
  Boost::boost
  Boost::date_time
  Boost::filesystem
  Boost::iostreams
  Boost::log
  Boost::regex
  Boost::system
  Boost::thread
  Boost::unit_test_framework
  Boost::wave)

add_test(NAME unittests COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittests_driver)
