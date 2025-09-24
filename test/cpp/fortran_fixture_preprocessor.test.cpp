#include "fortran_fixture_preprocessor.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <string>

/**
 * @brief Test fixture for FortranFixturePreprocessor behavior.
 */
class FortranFixturePreprocessorTest : public ::testing::Test {
protected:
    std::unordered_map<std::string, std::string> scope_map{
        {"test_fixture_t", "test"},
        {"suite_fixture_t", "suite"},
        {"session_fixture_t", "session"}
    };

    Fortest::FortranFixturePreprocessor preprocessor{scope_map};

    std::string process(const std::string &input) {
        return preprocessor.process(input);
    }
};

// -----------------------------------------------------------------------------
// Basic transformation
// -----------------------------------------------------------------------------

/**
 * @test Behavior: Generates setup/teardown routines for a test fixture type.
 */
TEST_F(FortranFixturePreprocessorTest, GeneratesTestFixtureRoutines) {
    std::string input = R"(
type :: test_fixture_t
   real, allocatable :: W(:)
end type test_fixture_t
)";
    std::string output = process(input);

    EXPECT_NE(output.find("subroutine setup_test_fixture"), std::string::npos);
    EXPECT_NE(output.find("subroutine teardown_test_fixture"), std::string::npos);
    EXPECT_NE(output.find("type(test_fixture_t), pointer :: fix"), std::string::npos);
    EXPECT_NE(output.find("call c_f_pointer(args, fix)"), std::string::npos);
}

/**
 * @test Behavior: Generates setup/teardown routines for a suite fixture type.
 */
TEST_F(FortranFixturePreprocessorTest, GeneratesSuiteFixtureRoutines) {
    std::string input = R"(
type :: suite_fixture_t
   real, allocatable :: A(:, :)
end type suite_fixture_t
)";
    std::string output = process(input);

    EXPECT_NE(output.find("subroutine setup_suite_fixture"), std::string::npos);
    EXPECT_NE(output.find("subroutine teardown_suite_fixture"), std::string::npos);
    EXPECT_NE(output.find("type(suite_fixture_t), pointer :: fix"), std::string::npos);
}

/**
 * @test Behavior: Generates setup/teardown routines for a session fixture type.
 */
TEST_F(FortranFixturePreprocessorTest, GeneratesSessionFixtureRoutines) {
    std::string input = R"(
type :: session_fixture_t
   integer :: id
end type session_fixture_t
)";
    std::string output = process(input);

    EXPECT_NE(output.find("subroutine setup_session_fixture"), std::string::npos);
    EXPECT_NE(output.find("subroutine teardown_session_fixture"), std::string::npos);
    EXPECT_NE(output.find("type(session_fixture_t), pointer :: fix"), std::string::npos);
}

/**
 * @test Behavior: Non-fixture code remains untouched.
 */
TEST_F(FortranFixturePreprocessorTest, LeavesNonFixtureCodeUnchanged) {
    std::string input = R"(
module helper_mod
   implicit none
   integer :: x = 42
end module helper_mod
)";
    std::string output = process(input);

    EXPECT_NE(output.find("module helper_mod"), std::string::npos);
    EXPECT_EQ(output.find("setup_"), std::string::npos);
    EXPECT_EQ(output.find("teardown_"), std::string::npos);
}
