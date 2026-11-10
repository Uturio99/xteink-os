add_executable(BookProgressTest
  BookProgressTest.cpp
  ${REPO_ROOT}/src/util/BookProgress.cpp
)
target_include_directories(BookProgressTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stubs ${REPO_ROOT}/src/util)
target_link_libraries(BookProgressTest PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(BookProgressTest)
