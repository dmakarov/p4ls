add_executable(p4lsd main.cpp)

target_compile_options(p4lsd PRIVATE
  "-g"
  "-Wall"
  "-Werror"
  "-Wextra"
  "-fvisibility=hidden"
  "-fvisibility-inlines-hidden")

target_include_directories(p4lsd
  PUBLIC
  ${PROJECT_SOURCE_DIR}/server/lsp
  ${PROJECT_SOURCE_DIR}/server/p4l)

target_link_libraries(p4lsd PUBLIC
  coverage_config
  p4l
  lsp
  Boost::boost
  Boost::date_time
  Boost::filesystem
  Boost::iostreams
  Boost::log
  Boost::regex
  Boost::system
  Boost::thread
  Boost::wave)

install(TARGETS p4lsd RUNTIME DESTINATION bin)
