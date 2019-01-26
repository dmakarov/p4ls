add_executable(p4lsd main.cpp)

target_compile_options(p4lsd PRIVATE "-Wall" "-Werror" "-Wextra" "-fvisibility=hidden" "-fvisibility-inlines-hidden")

target_include_directories(p4lsd
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src/lsp
  ${PROJECT_SOURCE_DIR}/src/p4l)

target_link_libraries(p4lsd
  p4l
  lsp
  Boost::boost
  Boost::log
  Boost::regex
  Boost::filesystem
  Boost::iostreams
  Boost::date_time
  Boost::thread
  Boost::system)

install(TARGETS p4lsd RUNTIME DESTINATION bin)
