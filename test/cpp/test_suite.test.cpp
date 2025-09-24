#include "test_suite.hpp"
#include "assert.hpp"
#include "logging.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>

using ::testing::HasSubstr;

// Simple ostream logger for capturing output
class OStreamLogger : public Fortest::Logger {
public:
    explicit OStreamLogger(std::ostream &out) : Logger(out), out_(out) {}
    void log(const std::string &msg, const std::string &tag) {
        out_ << "[" << tag << "] " << msg << "\n";
    }
private:
    std::ostream &out_;
};

class TestSuiteBehavior : public ::testing::Test {
protected:
    std::ostringstream buffer;
    std::shared_ptr<OStreamLogger> logger;
    Fortest::Assert<> assert_obj;

    void SetUp() override {
        logger = std::make_shared<OStreamLogger>(buffer);
        assert_obj.reset();
    }

    std::string get_output() const { return buffer.str(); }
    void clear_output() { buffer.str(""); buffer.clear(); }
};

/**
 * @brief Behavior: Suite returns its name correctly.
 */
TEST_F(TestSuiteBehavior, GetNameReturnsCorrectName) {
    Fortest::TestSuite<OStreamLogger> ts("AlphaSuite", assert_obj);
    EXPECT_EQ(ts.get_name(), "AlphaSuite");
}

/**
 * @brief Behavior: Adding and running a passing test records PASS status.
 */
TEST_F(TestSuiteBehavior, PassingTestReportsPass) {
    Fortest::TestSuite<OStreamLogger> ts("PassSuite", assert_obj);

    ts.add_test("always_pass", [&](void*, void*, void*) {
        assert_obj.assert_true(true);
    });

    ts.run(logger);

    auto statuses = ts.get_statuses();
    EXPECT_EQ(statuses.at("always_pass"), Fortest::Test::Status::PASS);

    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("Running test: always_pass"));
    EXPECT_THAT(out, HasSubstr("Test passed: always_pass"));
}

/**
 * @brief Behavior: Adding and running a failing test records FAIL status.
 */
TEST_F(TestSuiteBehavior, FailingTestReportsFail) {
    Fortest::TestSuite<OStreamLogger> ts("FailSuite", assert_obj);

    ts.add_test("always_fail", [&](void*, void*, void*) {
        assert_obj.assert_true(false);
    });

    ts.run(logger);

    auto statuses = ts.get_statuses();
    EXPECT_EQ(statuses.at("always_fail"), Fortest::Test::Status::FAIL);

    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("Running test: always_fail"));
    EXPECT_THAT(out, HasSubstr("Test failed: always_fail"));
}

/**
 * @brief Behavior: Suite-level fixture setup and teardown wrap all tests.
 */
TEST_F(TestSuiteBehavior, SuiteFixtureSetupAndTeardownCalled) {
    bool setup_called = false;
    bool teardown_called = false;

    Fortest::TestSuite<OStreamLogger> ts("WithFixture", assert_obj);

    Fortest::Fixture<void> suite_fixture(
        [&](void*) { setup_called = true; },
        [&](void*) { teardown_called = true; },
        nullptr,
        Fortest::Scope::Suite
    );
    ts.add_fixture(suite_fixture);

    ts.add_test("dummy", [&](void*, void*, void*) {
        assert_obj.assert_true(true);
    });

    ts.run(logger);

    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);
}

/**
 * @brief Behavior: If a suite fixture is added *after* registering tests,
 * it should still be applied to all tests (retroactive binding).
 */
TEST_F(TestSuiteBehavior, SuiteFixtureAddedAfterTestsAppliesToAllTests) {
    bool setup_called = false;
    bool teardown_called = false;

    Fortest::TestSuite<OStreamLogger> ts("RetroFixture", assert_obj);

    ts.add_test("test1", [&](void*, void*, void*) {
        assert_obj.assert_true(setup_called);
    });
    ts.add_test("test2", [&](void*, void*, void*) {
        assert_obj.assert_true(setup_called);
    });

    // Add suite fixture AFTER tests are already defined
    Fortest::Fixture<void> suite_fixture(
        [&](void*) { setup_called = true; },
        [&](void*) { teardown_called = true; },
        nullptr,
        Fortest::Scope::Suite
    );
    ts.add_fixture(suite_fixture);

    ts.run(logger);

    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);

    auto statuses = ts.get_statuses();
    EXPECT_EQ(statuses.at("test1"), Fortest::Test::Status::PASS);
    EXPECT_EQ(statuses.at("test2"), Fortest::Test::Status::PASS);
}
