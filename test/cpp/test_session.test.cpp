#include "test_session.hpp"
#include "assert.hpp"
#include "logging.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>

using ::testing::HasSubstr;

// Simple ostream logger
class OStreamLogger : public Fortest::Logger {
public:
    explicit OStreamLogger(std::ostream &out) : Logger(out), out_(out) {}
    void log(const std::string &msg, const std::string &tag) {
        out_ << "[" << tag << "] " << msg << "\n";
    }
private:
    std::ostream &out_;
};

class TestSessionBehavior : public ::testing::Test {
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
 * @brief Behavior: Adding a suite with a duplicate name throws an error.
 */
TEST_F(TestSessionBehavior, DuplicateSuiteThrows) {
    Fortest::TestSession<OStreamLogger> session(assert_obj);

    session.add_test_suite("Suite1");
    EXPECT_THROW(session.add_test_suite("Suite1"), std::runtime_error);
}

/**
 * @brief Behavior: Adding a test to a non-existent suite throws.
 */
TEST_F(TestSessionBehavior, AddTestToUnknownSuiteThrows) {
    Fortest::TestSession<OStreamLogger> session(assert_obj);

    EXPECT_THROW(
        session.add_test("NoSuchSuite", "dummy", [](void*,void*,void*){}),
        std::runtime_error
    );
}

/**
 * @brief Behavior: Session-level fixture setup and teardown run once per session.
 */
TEST_F(TestSessionBehavior, SessionFixtureSetupAndTeardownRunOnce) {
    bool setup_called = false;
    bool teardown_called = false;

    Fortest::TestSession<OStreamLogger> session(assert_obj);

    auto suite = &session.add_test_suite("Suite1");
    suite->add_test("dummy", [&](void*,void*,void*) {
        assert_obj.assert_true(true);
    });

    Fortest::Fixture<void> f(
        [&](void*) { setup_called = true; },
        [&](void*) { teardown_called = true; },
        nullptr,
        Fortest::Scope::Session
    );
    session.add_fixture(f);

    session.run(logger);

    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);
}

/**
 * @brief Behavior: Tests added to different suites are all executed.
 */
TEST_F(TestSessionBehavior, MultipleSuitesRunAllTests) {
    Fortest::TestSession<OStreamLogger> session(assert_obj);

    auto &suite1 = session.add_test_suite("Suite1");
    auto &suite2 = session.add_test_suite("Suite2");

    suite1.add_test("t1", [&](void*,void*,void*) {
        assert_obj.assert_true(true);
    });
    suite2.add_test("t2", [&](void*,void*,void*) {
        assert_obj.assert_true(true);
    });

    session.run(logger);

    auto status1 = session.get_test_suite_status("Suite1");
    auto status2 = session.get_test_suite_status("Suite2");

    EXPECT_EQ(status1["t1"], Fortest::Test::Status::PASS);
    EXPECT_EQ(status2["t2"], Fortest::Test::Status::PASS);
}

/**
 * @brief Behavior: Adding a non-session fixture with the session API throws.
 */
TEST_F(TestSessionBehavior, InvalidScopeForSessionFixtureThrows) {
    Fortest::TestSession<OStreamLogger> session(assert_obj);

    Fortest::Fixture<void> f(
        nullptr, nullptr, nullptr,
        Fortest::Scope::Test
    );
    EXPECT_THROW(session.add_fixture(f), std::runtime_error);
}

/**
 * @brief Behavior: Adding a fixture to a non-existent suite throws.
 */
TEST_F(TestSessionBehavior, AddFixtureToUnknownSuiteThrows) {
    Fortest::TestSession<OStreamLogger> session(assert_obj);

    Fortest::Fixture<void> f(
        nullptr, nullptr, nullptr,
        Fortest::Scope::Suite
    );
    EXPECT_THROW(session.add_fixture("NoSuchSuite", f), std::runtime_error);
}

/**
 * @brief Behavior: Statuses reflect passing and failing tests across suites.
 */
TEST_F(TestSessionBehavior, GetTestStatusesFromSession) {
    Fortest::TestSession<OStreamLogger> session(assert_obj);

    auto &suite = session.add_test_suite("Suite1");

    suite.add_test("pass", [&](void*,void*,void*) {
        assert_obj.assert_true(true);
    });
    suite.add_test("fail", [&](void*,void*,void*) {
        assert_obj.assert_true(false);
    });

    session.run(logger);

    auto statuses = session.get_test_suite_status("Suite1");
    EXPECT_EQ(statuses["pass"], Fortest::Test::Status::PASS);
    EXPECT_EQ(statuses["fail"], Fortest::Test::Status::FAIL);
}

/**
 * @brief Behavior: A session fixture added after suites and tests
 * are defined should still be applied to all tests.
 */
TEST_F(TestSessionBehavior, SessionFixtureAppliesRetroactivelyToExistingTests) {
    bool setup_called = false;
    bool teardown_called = false;

    Fortest::TestSession<OStreamLogger> session(assert_obj);

    // Add a suite and some tests before fixture is added
    auto &suite = session.add_test_suite("PreFixtureSuite");
    suite.add_test("test1", [&](void*, void*, void*) {
        // Should see session fixture setup has run
        assert_obj.assert_true(setup_called);
    });
    suite.add_test("test2", [&](void*, void*, void*) {
        assert_obj.assert_true(setup_called);
    });

    // Now add the session fixture
    Fortest::Fixture<void> f(
        [&](void*) { setup_called = true; },
        [&](void*) { teardown_called = true; },
        nullptr,
        Fortest::Scope::Session
    );
    session.add_fixture(f);

    session.run(logger);

    // Fixture should have run exactly once for the session
    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);

    // Both tests should have passed
    auto statuses = session.get_test_suite_status("PreFixtureSuite");
    EXPECT_EQ(statuses["test1"], Fortest::Test::Status::PASS);
    EXPECT_EQ(statuses["test2"], Fortest::Test::Status::PASS);
}

