add_executable(BookProgressTest
  BookProgressTest.cpp
  ${REPO_ROOT}/src/util/BookProgress.cpp
  ${REPO_ROOT}/lib/Utf8/Utf8.cpp
)
target_include_directories(BookProgressTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/stubs
  ${REPO_ROOT}/src/util
  ${REPO_ROOT}/lib/Utf8
)
target_link_libraries(BookProgressTest PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(BookProgressTest)
