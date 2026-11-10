add_executable(XtcChaptersTest
  XtcChaptersTest.cpp
  ${REPO_ROOT}/lib/Xtc/Xtc/XtcParser.cpp
)

target_include_directories(XtcChaptersTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/stubs
  ${REPO_ROOT}/lib/Xtc
  ${REPO_ROOT}/lib/Xtc/Xtc
)

target_link_libraries(XtcChaptersTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(XtcChaptersTest)
