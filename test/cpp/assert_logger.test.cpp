#include "assert_logger.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>

using ::testing::HasSubstr;
using ::testing::Not;

/**
 * @brief Test fixture for AssertLogger behavior.
 *
 * Redirects logger output to an ostringstream buffer
 * so the test can inspect exactly what is printed.
 */
class AssertLoggerTest : public ::testing::Test {
protected:
    std::ostringstream buffer;
    Fortest::AssertLogger logger{buffer, true}; ///< Logger with colors enabled

    /// @brief Retrieve accumulated log output.
    std::string get_output() { return buffer.str(); }

    /// @brief Clear the log buffer.
    void clear_output() {
        buffer.str("");
        buffer.clear();
    }
};

// -----------------------------------------------------------------------------
// Basic logging behavior
// -----------------------------------------------------------------------------

/**
 * @test Behavior: "PASS" logs with green color and [ASSERT][PASS] prefix.
 */
TEST_F(AssertLoggerTest, LogPass) {
    logger.log("expected equal", "PASS");
    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("[ASSERT][PASS] expected equal"));
    EXPECT_THAT(out, HasSubstr("\033[32m")); // green
    EXPECT_EQ(logger.entries().back().tag, "PASS");
}

/**
 * @test Behavior: "FAIL" logs with red color and [ASSERT][FAIL] prefix.
 */
TEST_F(AssertLoggerTest, LogFail) {
    logger.log("expected not equal", "FAIL");
    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("[ASSERT][FAIL] expected not equal"));
    EXPECT_THAT(out, HasSubstr("\033[31m")); // red
    EXPECT_EQ(logger.entries().back().tag, "FAIL");
}

/**
 * @test Behavior: Unknown tag is logged with yellow color and [ASSERT] prefix.
 */
TEST_F(AssertLoggerTest, LogUnknownTag) {
    logger.log("misc info", "OTHER");
    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("[ASSERT][OTHER] misc info"));
    EXPECT_THAT(out, HasSubstr("\033[33m")); // yellow
    EXPECT_EQ(logger.entries().back().tag, "OTHER");
}

// -----------------------------------------------------------------------------
// Entries tracking
// -----------------------------------------------------------------------------

/**
 * @test Behavior: Logger stores entries internally for later inspection.
 */
TEST_F(AssertLoggerTest, StoresEntries) {
    logger.log("first pass", "PASS");
    logger.log("second fail", "FAIL");

    const auto &entries = logger.entries();
    ASSERT_EQ(entries.size(), 2u);
    EXPECT_EQ(entries[0].tag, "PASS");
    EXPECT_EQ(entries[0].msg, "first pass");
    EXPECT_EQ(entries[1].tag, "FAIL");
    EXPECT_EQ(entries[1].msg, "second fail");
}

/**
 * @test Behavior: Entries accumulate in order of logging.
 */
TEST_F(AssertLoggerTest, EntriesAccumulateInOrder) {
    logger.log("one", "PASS");
    logger.log("two", "FAIL");
    logger.log("three", "OTHER");

    const auto &entries = logger.entries();
    ASSERT_EQ(entries.size(), 3u);
    EXPECT_EQ(entries[0].msg, "one");
    EXPECT_EQ(entries[1].msg, "two");
    EXPECT_EQ(entries[2].msg, "three");
}

// -----------------------------------------------------------------------------
// Summary behavior
// -----------------------------------------------------------------------------

/**
 * @test Behavior: print_summary prints pass/fail counts.
 */
TEST_F(AssertLoggerTest, SummaryReportsCounts) {
    logger.log("ok", "PASS");
    logger.log("bad", "FAIL");
    clear_output();

    logger.print_summary();
    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("Assertions Summary: 1 passed, 1 failed"));
}

/**
 * @test Behavior: print_summary counts multiple entries correctly.
 */
TEST_F(AssertLoggerTest, SummaryCountsMultiple) {
    logger.log("pass1", "PASS");
    logger.log("pass2", "PASS");
    logger.log("fail1", "FAIL");
    clear_output();

    logger.print_summary();
    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("2 passed, 1 failed"));
}

// -----------------------------------------------------------------------------
// Color control
// -----------------------------------------------------------------------------

/**
 * @test Behavior: When colors are disabled, no ANSI codes appear.
 */
TEST_F(AssertLoggerTest, NoColorsWhenDisabled) {
    Fortest::AssertLogger plain_logger(buffer, false);
    plain_logger.log("plain", "PASS");

    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("[ASSERT][PASS] plain"));
    EXPECT_THAT(out, Not(HasSubstr("\033[32m"))); // no green
}
