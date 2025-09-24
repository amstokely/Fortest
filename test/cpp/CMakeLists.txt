include(FetchContent)
FETCHCONTENT_DECLARE(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FETCHCONTENT_MAKEAVAILABLE(googletest)

add_executable(test_assert assert.test.cpp)
target_link_libraries(test_assert PUBLIC GTest::gtest_main GTest::gmock cpp_fortest)
add_test(NAME test_assert COMMAND test_assert)

add_executable(test_logging logging.test.cpp)
target_link_libraries(test_logging PUBLIC GTest::gtest_main GTest::gmock cpp_fortest)
add_test(NAME test_logging COMMAND test_logging)

add_executable(test_assert_logger assert_logger.test.cpp)
target_link_libraries(test_assert_logger PUBLIC GTest::gtest_main GTest::gmock
                                            cpp_fortest)
add_test(NAME test_assert_logger COMMAND test_assert_logger)

add_executable(test_test_suite test_suite.test.cpp)
target_link_libraries(test_test_suite PUBLIC GTest::gtest_main GTest::gmock cpp_fortest)
add_test(NAME test_test_suite COMMAND test_test_suite)

add_executable(test_test test.test.cpp)
target_link_libraries(test_test PUBLIC GTest::gtest_main GTest::gmock cpp_fortest)
add_test(NAME test_test COMMAND test_test)

add_executable(test_test_session test_session.test.cpp)
target_link_libraries(test_test_session PUBLIC GTest::gtest_main GTest::gmock cpp_fortest)
add_test(NAME test_test_session COMMAND test_test_session)

add_executable(test_fixture fixture.test.cpp)
target_link_libraries(test_fixture PUBLIC GTest::gtest_main GTest::gmock cpp_fortest)
add_test(NAME test_fixture COMMAND test_fixture)

add_executable(test_fortran_test_preprocessor fortran_test_preprocessor.test.cpp)
target_link_libraries(test_fortran_test_preprocessor PUBLIC GTest::gtest_main GTest::gmock
                                                cpp_fortest)
add_test(NAME test_fortran_test_preprocessor COMMAND test_fortran_test_preprocessor)

add_executable(test_fortran_fixture_preprocessor fortran_fixture_preprocessor.test.cpp)
target_link_libraries(test_fortran_fixture_preprocessor PUBLIC GTest::gtest_main GTest::gmock cpp_fortest)
add_test(NAME test_fortran_fixture_preprocessor COMMAND test_fortran_fixture_preprocessor)

add_executable(test_fortran_fixture_extractor fortran_fixture_extractor.test.cpp)
target_link_libraries(test_fortran_fixture_extractor PUBLIC GTest::gtest_main GTest::gmock
                                                cpp_fortest)
add_test(NAME test_fortran_fixture_extractor COMMAND test_fortran_fixture_extractor)