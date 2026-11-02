add_executable(X4ProHomeLayoutTest X4ProHomeLayoutTest.cpp)
target_include_directories(X4ProHomeLayoutTest PRIVATE
  ${REPO_ROOT}/src
  ${REPO_ROOT}/freeink-sdk/libs/ui/FreeInkUI/include
)
target_link_libraries(X4ProHomeLayoutTest PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(X4ProHomeLayoutTest)
