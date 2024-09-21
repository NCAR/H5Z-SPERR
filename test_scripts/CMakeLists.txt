add_executable(        bitmask_compactor_test bitmask_compactor_test.cpp )
target_link_libraries( bitmask_compactor_test PUBLIC h5z-sperr GTest::gtest_main )

include(GoogleTest)
gtest_discover_tests( bitmask_compactor_test )
