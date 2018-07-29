include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_executable(p4lsd main.cpp)

target_link_libraries(p4lsd
  p4ls
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
  ${LIBGMP_LIBRARIES})

install(TARGETS p4lsd RUNTIME DESTINATION bin)
