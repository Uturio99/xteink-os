add_executable(VietnameseFontCoverageTest
  VietnameseFontCoverageTest.cpp
  ${REPO_ROOT}/lib/EpdFont/EpdFont.cpp
  ${REPO_ROOT}/lib/InflateReader/InflateReader.cpp
  ${REPO_ROOT}/lib/Utf8/Utf8.cpp
  ${REPO_ROOT}/lib/uzlib/src/tinflate.c
  ${REPO_ROOT}/freeink-sdk/libs/font/FreeInkFont/src/TtfFont.cpp
)

find_package(ZLIB REQUIRED)

target_include_directories(VietnameseFontCoverageTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/generated
  ${REPO_ROOT}/lib/EpdFont
  ${REPO_ROOT}/lib/Utf8
  ${REPO_ROOT}/lib/Memory
  ${REPO_ROOT}/lib/InflateReader
  ${REPO_ROOT}/lib/uzlib/src
  ${REPO_ROOT}/freeink-sdk/libs/font/FreeInkFont/include
  ${REPO_ROOT}/freeink-sdk/libs/font/FreeInkFont/third_party/stb
)

target_compile_definitions(VietnameseFontCoverageTest PRIVATE
  WP1_SERIF_TTF="${REPO_ROOT}/lib/EpdFont/builtinFonts/source/NotoSerif/NotoSerif-Regular.ttf"
)

target_link_libraries(VietnameseFontCoverageTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
  ZLIB::ZLIB
)

target_compile_options(VietnameseFontCoverageTest PRIVATE -fno-exceptions)
gtest_discover_tests(VietnameseFontCoverageTest)
