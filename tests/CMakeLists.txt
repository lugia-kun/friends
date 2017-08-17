
add_library(libtestfriends STATIC friends_test.c)
target_link_libraries(libtestfriends libfriends)
set_target_properties(libtestfriends PROPERTIES
  INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};${LIBFRIENDS_INCLUDE_DIR}"
  OUTPUT_NAME "testfriends")

function(add_friends_test NAME)
  set(L ${ARGV})
  list(REMOVE_AT L 0)
  add_executable(${NAME} ${L})
  target_link_libraries(${NAME} ${${NAME}_LIBRARIES} libfriends libtestfriends)
  set_target_properties(${NAME} PROPERTIES
    INCLUDE_DIRECTORIES
    "${${NAME}_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};${LIBFRIENDS_INCLUDE_DIR}")
  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
endfunction()

add_friends_test(friends_list list.c)
add_friends_test(friends_set  set.c)
