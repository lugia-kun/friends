
add_library(libtestfriends STATIC friends_test.c)
target_link_libraries(libtestfriends libfriends)
set_target_properties(libtestfriends PROPERTIES
  INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}"
  OUTPUT_NAME "testfriends")

function(add_friends_test_executable NAME)
  set(L ${ARGV})
  list(REMOVE_AT L 0)
  add_executable(${NAME} ${L})
  target_link_libraries(${NAME} ${${NAME}_LIBRARIES} libfriends libtestfriends)
  set_target_properties(${NAME} PROPERTIES
    INCLUDE_DIRECTORIES
    "${${NAME}_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

function(add_friends_test NAME)
  set(L ${ARGV})
  list(REMOVE_AT L 0)
  add_friends_test_executable(${NAME} ${L})
  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
endfunction()

add_friends_test(friends_string string.c)
add_friends_test(friends_list list.c)
add_friends_test(friends_set  set.c)
add_friends_test(friends_encoding encoding.c)
add_friends_test_executable(friends_lexer lexer.c)

add_executable(friends_rbtree rbtree.c)
set_target_properties(friends_rbtree PROPERTIES
  INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../lib;${CMAKE_CURRENT_BINARY_DIR}/../lib" )
