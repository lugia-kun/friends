
add_executable(friends main.c)
target_link_libraries(friends libfriends)

set_target_properties(friends PROPERTIES
  INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};${LIBFRIENDS_INCLUDE_DIR}")
