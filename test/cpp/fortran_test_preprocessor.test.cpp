#include "fortran_test_preprocessor.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <string>

/**
 * @brief Test fixture for FortranTestPreprocessor behavior.
 */
class FortranTestPreprocessorTest : public ::testing::Test {
protected:
    std::unordered_map<std::string, std::string> scope_map {
        {"test_fixture_t", "test"},
        {"suite_fixture_t", "suite"},
        {"session_fixture_t", "session"}
    };

    Fortest::FortranTestPreprocessor preprocessor{scope_map};

    std::string process(const std::string &input) {
        return preprocessor.process(input);
    }
};

// -----------------------------------------------------------------------------
// Basic transformation
// -----------------------------------------------------------------------------

/**
 * @test Behavior: A simple @test subroutine is wrapped with c_ptr
 * arguments and an _impl subroutine.
 */
TEST_F(FortranTestPreprocessorTest, WrapsSimpleTestSubroutine) {
    std::string input = R"(
@test
   subroutine test_normalize_vector_length(t, ts)
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      real :: U_norm(3), length

      call normalize_vector(ts%U, U_norm)
      length = sqrt(sum(U_norm * U_norm))

      call assert_equal(length, 1.0, abs_tol=1.0e-5)
   end subroutine test_normalize_vector_length
)";
    std::string output = process(input);

    EXPECT_NE(output.find("subroutine test_normalize_vector_length(t_ptr, ts_ptr, s_ptr)"),
              std::string::npos);
    EXPECT_NE(output.find("call test_normalize_vector_length_impl(t, ts)"),
              std::string::npos);
    EXPECT_NE(output.find("subroutine test_normalize_vector_length_impl(t, ts)"),
              std::string::npos);
}

/**
 * @test Behavior: Non-test subroutines remain untouched.
 */
TEST_F(FortranTestPreprocessorTest, LeavesNonTestSubroutinesUnchanged) {
    std::string input = R"(
subroutine helper_routine(x)
   integer :: x
   print *, x
end subroutine helper_routine
)";
    std::string output = process(input);

    EXPECT_NE(output.find("subroutine helper_routine(x)"), std::string::npos);
    EXPECT_EQ(output.find("_impl"), std::string::npos); // no impl generated
}

/**
 * @test Behavior: Multiple @test subroutines are all wrapped.
 */
TEST_F(FortranTestPreprocessorTest, HandlesMultipleTestSubroutines) {
    std::string input = R"(
@test
   subroutine test_one(a)
      integer :: a
      print *, a
   end subroutine test_one

@test
   subroutine test_two(b)
      integer :: b
      print *, b
   end subroutine test_two
)";
    std::string output = process(input);

    EXPECT_NE(output.find("subroutine test_one_impl"), std::string::npos);
    EXPECT_NE(output.find("subroutine test_two_impl"), std::string::npos);
}

/**
 * @test Behavior: Wrapper adds c_f_pointer calls for t and ts arguments.
 */
TEST_F(FortranTestPreprocessorTest, AddsCFPointerCalls) {
    std::string input = R"(
@test
   subroutine test_with_pointers(t, ts)
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
   end subroutine test_with_pointers
)";
    std::string output = process(input);

    EXPECT_NE(output.find("call c_f_pointer(t_ptr, t)"), std::string::npos);
    EXPECT_NE(output.find("call c_f_pointer(ts_ptr, ts)"), std::string::npos);
}

// -----------------------------------------------------------------------------
// Fixture scope behavior
// -----------------------------------------------------------------------------

/**
 * @test Behavior: Test fixture declarations should cause deref of t_ptr.
 */
TEST_F(FortranTestPreprocessorTest, UsesTestFixtureDerefsTptr) {
    std::string input = R"(
@test
   subroutine test_with_test_fixture(t)
      type(test_fixture_t), pointer :: t
   end subroutine test_with_test_fixture
)";
    std::string output = process(input);

    EXPECT_NE(output.find("call c_f_pointer(t_ptr, t)"), std::string::npos)
        << "Expected test fixture to deref t_ptr into t";
}

/**
 * @test Behavior: Suite fixture declarations should cause deref of ts_ptr.
 */
TEST_F(FortranTestPreprocessorTest, UsesSuiteFixtureDerefsTsPtr) {
    std::string input = R"(
@test
   subroutine test_with_suite_fixture(ts)
      type(suite_fixture_t), pointer :: ts
   end subroutine test_with_suite_fixture
)";
    std::string output = process(input);

    EXPECT_NE(output.find("call c_f_pointer(ts_ptr, ts)"), std::string::npos)
        << "Expected suite fixture to deref ts_ptr into ts";
}

/**
 * @test Behavior: Session fixture declarations should cause deref of s_ptr.
 */
TEST_F(FortranTestPreprocessorTest, UsesSessionFixtureDerefsSptr) {
    std::string input = R"(
@test
   subroutine test_with_session_fixture(s)
      type(session_fixture_t), pointer :: s
   end subroutine test_with_session_fixture
)";
    std::string output = process(input);

    EXPECT_NE(output.find("call c_f_pointer(s_ptr, s)"), std::string::npos)
        << "Expected session fixture to deref s_ptr into s";
}

/**
 * @test Behavior: If no fixtures are declared, no c_f_pointer calls are emitted.
 */
TEST_F(FortranTestPreprocessorTest, NoFixturesNoDerefs) {
    std::string input = R"(
@test
   subroutine test_without_fixtures()
      integer :: x
      x = 1
   end subroutine test_without_fixtures
)";
    std::string output = process(input);

    EXPECT_EQ(output.find("call c_f_pointer("), std::string::npos)
        << "Expected no c_f_pointer derefs when no fixtures declared";
}
