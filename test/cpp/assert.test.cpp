// test_assert.cpp
#include "assert.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>


// Null logger that satisfies LoggerLike but discards output
class NullLogger {
public:
    static void log(const std::string &, const std::string &) {}
};

using TestAssert = Fortest::Assert<NullLogger>;

// Base fixture
class AssertTest : public ::testing::Test {
protected:
    std::shared_ptr<NullLogger> null_logger;
    TestAssert test_assert;

    void SetUp() override {
        null_logger = std::make_shared<NullLogger>();
        test_assert.reset();
    }

    void expect_summary(const int expected_pass, const int expected_fail) const {
        EXPECT_EQ(test_assert.get_num_passed(), expected_pass);
        EXPECT_EQ(test_assert.get_num_failed(), expected_fail);
    }
};

// -------- Equality & Inequality --------

/// @test AssertEqual passes when two integers are the same.
TEST_F(AssertTest, Equal_PassesWhenValuesAreSame_Int) {
    test_assert.assert_equal(42, 42);
    expect_summary(1, 0);
}

/// @test AssertEqual fails when two integers are different.
TEST_F(AssertTest, Equal_FailsWhenValuesDiffer_Int) {
    test_assert.assert_equal(1, 2);
    expect_summary(0, 1);
}

/// @test AssertEqual passes when two strings are identical.
TEST_F(AssertTest, Equal_PassesWhenValuesAreSame_String) {
    test_assert.assert_equal(std::string("abc"), std::string("abc"));
    expect_summary(1, 0);
}

/// @test AssertEqual fails when two strings differ.
TEST_F(AssertTest, Equal_FailsWhenValuesDiffer_String) {
    test_assert.assert_equal(std::string("abc"), std::string("def"));
    expect_summary(0, 1);
}

/// @test AssertNotEqual passes when two values are different.
TEST_F(AssertTest, NotEqual_PassesWhenValuesDiffer) {
    test_assert.assert_not_equal(1, 2);
    expect_summary(1, 0);
}

/// @test AssertNotEqual fails when two values are the same.
TEST_F(AssertTest, NotEqual_FailsWhenValuesAreSame) {
    test_assert.assert_not_equal(5, 5);
    expect_summary(0, 1);
}

// -------- Boolean Assertions --------

/// @test AssertTrue passes when the condition is true.
TEST_F(AssertTest, True_PassesOnTrueCondition) {
    test_assert.assert_true(true);
    expect_summary(1, 0);
}

/// @test AssertTrue fails when the condition is false.
TEST_F(AssertTest, True_FailsOnFalseCondition) {
    test_assert.assert_true(false);
    expect_summary(0, 1);
}

/// @test AssertFalse passes when the condition is false.
TEST_F(AssertTest, False_PassesOnFalseCondition) {
    test_assert.assert_false(false);
    expect_summary(1, 0);
}

/// @test AssertFalse fails when the condition is true.
TEST_F(AssertTest, False_FailsOnTrueCondition) {
    test_assert.assert_false(true);
    expect_summary(0, 1);
}

// -------- Accumulation & Reset --------

/// @test Multiple assertions accumulate pass and fail counts correctly.
TEST_F(AssertTest, MultipleAssertionsAccumulateResults) {
    test_assert.assert_equal(1, 1);      // pass
    test_assert.assert_equal(1, 2);      // fail
    test_assert.assert_not_equal(5, 5);  // fail
    test_assert.assert_true(true);       // pass

    expect_summary(2, 2);
}

/// @test Reset clears both passed and failed counters.
TEST_F(AssertTest, ResetClearsPreviousResults) {
    test_assert.assert_equal(1, 2); // fail
    test_assert.assert_true(true);  // pass
    expect_summary(1, 1);

    test_assert.reset();
    expect_summary(0, 0);
}

/// @test Reset clears counters after all assertions passed.
TEST_F(AssertTest, ResetAfterAllPasses) {
    test_assert.assert_equal(1, 1);
    test_assert.assert_true(true);
    expect_summary(2, 0);

    test_assert.reset();
    expect_summary(0, 0);
}

/// @test Reset clears counters after all assertions failed.
TEST_F(AssertTest, ResetAfterAllFails) {
    test_assert.assert_equal(1, 2);
    test_assert.assert_false(true);
    expect_summary(0, 2);

    test_assert.reset();
    expect_summary(0, 0);
}

// -------- Edge Cases --------

/// @test AssertEqual treats two empty strings as equal.
TEST_F(AssertTest, EqualHandlesEmptyStrings) {
    test_assert.assert_equal(std::string(""), std::string(""));
    expect_summary(1, 0);
}

/// @test AssertNotEqual passes when comparing empty vs non-empty string.
TEST_F(AssertTest, NotEqualHandlesEmptyVsNonEmptyString) {
    test_assert.assert_not_equal(std::string(""), std::string("x"));
    expect_summary(1, 0);
}

/// @test AssertEqual works with floating-point values.
TEST_F(AssertTest, EqualHandlesFloatingPointValues) {
    test_assert.assert_equal(3.14, 3.14);
    expect_summary(1, 0);
}

/// @test AssertNotEqual works with different floating-point values.
TEST_F(AssertTest, NotEqualHandlesFloatingPointValues) {
    test_assert.assert_not_equal(3.14, 2.71);
    expect_summary(1, 0);
}

/// @test AssertEqual with absolute tolerance passes for close doubles.
TEST_F(AssertTest, Equal_PassesWithinAbsoluteTolerance) {
    double a = 1.0000001;
    double b = 1.0;
    double abs_tol = 1e-5;

    test_assert.assert_equal(a, b, abs_tol, 0.0);
    expect_summary(1, 0);
}

/// @test AssertEqual with absolute tolerance fails when difference is too large.
TEST_F(AssertTest, Equal_FailsOutsideAbsoluteTolerance) {
    double a = 1.1;
    double b = 1.0;
    double abs_tol = 1e-3;

    test_assert.assert_equal(a, b, abs_tol, 0.0);
    expect_summary(0, 1);
}

/// @test AssertEqual with relative tolerance passes for proportionally close values.
TEST_F(AssertTest, Equal_PassesWithinRelativeTolerance) {
    double a = 1000.0;
    double b = 1001.0;
    double rel_tol = 0.01; // within 1%

    test_assert.assert_equal(a, b, 0.0, rel_tol);
    expect_summary(1, 0);
}

/// @test AssertEqual with relative tolerance fails when values differ too much.
TEST_F(AssertTest, Equal_FailsOutsideRelativeTolerance) {
    double a = 1000.0;
    double b = 1050.0;
    double rel_tol = 0.01; // 1% tolerance not enough

    test_assert.assert_equal(a, b, 0.0, rel_tol);
    expect_summary(0, 1);
}
