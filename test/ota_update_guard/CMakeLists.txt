add_executable(OtaUpdateGuardTest
  OtaUpdateGuardTest.cpp
  ${REPO_ROOT}/src/network/FirmwareBoardTag.cpp
  ${REPO_ROOT}/lib/JsonParser/ReleaseJsonParser.cpp
  ${REPO_ROOT}/freeink-sdk/libs/network/JsonSax/src/StreamingJsonParser.cpp
)

target_compile_definitions(OtaUpdateGuardTest PRIVATE
  FREEINK_DEVICE_X4PRO=1
  FREEINK_DEVICE_X4CLASSIC=0
  FREEINK_DEVICE_METALIO_EINK4=0
  FREEINK_DEVICE_X4=0
  FREEINK_DEVICE_X3=0
  FREEINK_DEVICE_PAPERMONO=0
  FREEINK_DEVICE_STICKY=0
  FREEINK_DEVICE_M5PAPER=0
  FREEINK_DEVICE_EEGO_A4=0
  FREEINK_DEVICE_PICCO=0
  FREEINK_DEVICE_MURPHY_M4=0
  FREEINK_DEVICE_LILYGO=0
  FREEINK_DEVICE_M5=0
  FREEINK_DEVICE_MURPHY=0
  FREEINK_DEVICE_DELINK=0
)

target_include_directories(OtaUpdateGuardTest PRIVATE
  ${REPO_ROOT}/src
  ${REPO_ROOT}/lib/JsonParser
  ${REPO_ROOT}/freeink-sdk/libs/display/FreeInkDisplay/test/host/pro_stubs
  ${REPO_ROOT}/freeink-sdk/libs/network/JsonSax/include
)

target_link_libraries(OtaUpdateGuardTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(OtaUpdateGuardTest)
