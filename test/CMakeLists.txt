
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        test_gtest
        parser/test_gtest.cpp
)
target_link_libraries(
        test_gtest
        gtest_main
)

add_executable(
        test_request
        parser/test_request.cpp
        ../src/parser/request.cpp
        ../src/util/util.cpp
)
target_link_libraries(
        test_request
        gtest_main
)

add_executable(
        test_thread_pool
        thread_pool/test_thread_pool.cpp
        ../src/thread_pool/thread_pool.cpp
        ../src/util/util.cpp
)
target_link_libraries(
        test_thread_pool
        gtest_main
        pthread
)

add_executable(
        test_parser
        parser/test_parser.cpp
        ../src/parser/parser.cpp
        ../src/parser/request.cpp
        ../src/util/util.cpp
)
target_link_libraries(
        test_parser
        gtest_main
)

add_executable(
        test_logger
        log/test_logger.cpp
        ../src/log/logger.cpp
        ../src/util/util.cpp
)
target_link_libraries(
        test_logger
        gtest_main
        pthread
)


include(GoogleTest)
#gtest_discover_tests(test_gtest)
#gtest_discover_tests(test_request)
#gtest_discover_tests(test_thread_pool)
gtest_discover_tests(test_parser)
gtest_discover_tests(test_logger)
