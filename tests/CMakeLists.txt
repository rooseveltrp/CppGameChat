find_package(GTest QUIET)

if(GTest_FOUND)
    add_subdirectory(core_tests)
else()
    message(STATUS "GoogleTest not found, skipping tests")
endif()