add_executable(        bitmask_compactor_test bitmask_compactor_test.cpp )
target_link_libraries( bitmask_compactor_test PUBLIC h5z-sperr GTest::gtest_main )

add_executable(        bitstream_test bitstream_test.cpp )
target_link_libraries( bitstream_test PUBLIC h5z-sperr GTest::gtest_main )

include(GoogleTest)
gtest_discover_tests( bitmask_compactor_test )
gtest_discover_tests( bitstream_test )
