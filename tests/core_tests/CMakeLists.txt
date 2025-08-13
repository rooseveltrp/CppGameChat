add_executable(core_tests
    test_presence.cpp
    test_retry.cpp
    test_mapper.cpp
)

target_link_libraries(core_tests CppGameChatCore GTest::gtest GTest::gtest_main)

enable_testing()
add_test(NAME core_tests COMMAND core_tests)