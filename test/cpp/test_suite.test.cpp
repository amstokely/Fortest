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
    void log(const std::string &msg, const std::string &tag,
             const std::optional<std::string> &border = std::nullopt) {
        out_ << "[" << tag << "] " << msg << "\n";
    }
private:
    std::ostream &out_;
};

class TestSuiteBehavior : public ::testing::Test {
protected:
    std::ostringstream buffer;
    std::shared_ptr<OStreamLogger> logger;
    Fortest::Assert<OStreamLogger> assert_obj{static_cast<std::ostream&>(buffer)};

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

/**
 * @brief Behavior: Adding and running a parameterized test records status for all indices.
 */
TEST_F(TestSuiteBehavior, ParameterizedTestReportsStatuses) {
    Fortest::TestSuite<OStreamLogger> ts("ParamSuite", assert_obj);

    ts.register_parameterized_test("parity_test",
        [&](void*, void*, void*, int idx) {
            assert_obj.assert_true(idx % 2 == 0); // pass for even, fail for odd
        },
        {0, 1, 2}
    );

    ts.run(logger);

    auto statuses = ts.get_statuses();
    EXPECT_EQ(statuses.at("parity_test"), Fortest::Test::Status::FAIL); // because idx=1 fails
}

/**
 * @brief Behavior: A parameterized test that passes all cases reports PASS.
 */
TEST_F(TestSuiteBehavior, ParameterizedTestAllPass) {
    Fortest::TestSuite<OStreamLogger> ts("ParamPassSuite", assert_obj);

    ts.register_parameterized_test("all_pass",
        [&](void*, void*, void*, int) {
            assert_obj.assert_true(true);
        },
        {0, 1}
    );

    ts.run(logger);

    auto statuses = ts.get_statuses();
    EXPECT_EQ(statuses.at("all_pass"), Fortest::Test::Status::PASS);

    std::string out = get_output();
    EXPECT_THAT(out, HasSubstr("Running parameterized test: all_pass"));
    EXPECT_THAT(out, HasSubstr("Parameterized test passed: all_pass"));
}

/**
 * @brief Behavior: Parameterized tests inherit suite fixtures just like regular tests.
 */
TEST_F(TestSuiteBehavior, ParameterizedTestInheritsSuiteFixture) {
    bool setup_called = false;

    Fortest::TestSuite<OStreamLogger> ts("ParamFixtureSuite", assert_obj);

    Fortest::Fixture<void> suite_fixture(
        [&](void*) { setup_called = true; },
        nullptr,
        nullptr,
        Fortest::Scope::Suite
    );
    ts.add_fixture(suite_fixture);

    ts.register_parameterized_test("fixture_check",
        [&](void*, void*, void*, int) {
            assert_obj.assert_true(setup_called);
        },
        {0}
    );

    ts.run(logger);

    EXPECT_TRUE(setup_called);
    auto statuses = ts.get_statuses();
    EXPECT_EQ(statuses.at("fixture_check"), Fortest::Test::Status::PASS);
}
