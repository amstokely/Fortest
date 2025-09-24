#include "test_suite.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>

using ::testing::HasSubstr;

// A simple logger for testing that writes into an ostringstream
class OStreamLogger : public Fortest::Logger {
public:
    explicit OStreamLogger(std::ostream &out) : Logger(out) {}
};

class TestFixture : public ::testing::Test {
protected:
    std::ostringstream buffer;
    std::shared_ptr<OStreamLogger> logger;
    Fortest::Assert<> assert_obj; // for Test

    void SetUp() override {
        logger = std::make_shared<OStreamLogger>(buffer);
        assert_obj.reset();
    }

    std::string get_output() const { return buffer.str(); }

    void clear_output() {
        buffer.str("");
        buffer.clear();
    }
};

/**
 * @brief Behavior: A test stores and returns its name.
 */
TEST_F(TestFixture, ConstructorSetsName) {
    const std::string test_name = "test";
    const Fortest::Test test(test_name, Fortest::TestFunction());
    EXPECT_EQ(test_name, test.get_name());
}

/**
 * @brief Behavior: A test can run with fixtures at all scopes.
 */
TEST_F(TestFixture, RunTestFunctionWithFixtures) {
    auto setup = [](void *a) {
        auto int_a = static_cast<int *>(a);
        *int_a = 0;
    };
    auto teardown = [](void *) {};
    auto test_args = new int(1);
    auto suite_args = new int(1);
    auto session_args = new int(1);

    auto test_fixture = std::make_shared<Fortest::Fixture<void>>(
        setup, teardown, test_args, Fortest::Scope::Test);
    auto suite_fixture = std::make_shared<Fortest::Fixture<void>>(
        nullptr, nullptr, suite_args, Fortest::Scope::Suite);
    auto session_fixture = std::make_shared<Fortest::Fixture<void>>(
        nullptr, nullptr, session_args, Fortest::Scope::Session);

    auto fn = [](void *a, void *b, void *c) {
        auto int_a = static_cast<int *>(a);
        auto int_b = static_cast<int *>(b);
        auto int_c = static_cast<int *>(c);
        *int_a = *int_b + *int_c;
    };

    Fortest::Test test("add", fn);
    test.add_fixture(test_fixture);
    test.add_fixture(suite_fixture);
    test.add_fixture(session_fixture);

    test.run(logger, assert_obj);

    EXPECT_EQ(*test_args, *suite_args + *session_args);

    delete test_args;
    delete suite_args;
    delete session_args;
}

/**
 * @brief Behavior: A test created with an empty name reports empty.
 */
TEST_F(TestFixture, ConstructorWithEmptyName) {
    Fortest::Test test("", Fortest::TestFunction());
    EXPECT_TRUE(test.get_name().empty());
}

/**
 * @brief Behavior: A test runs correctly with no fixtures.
 */
TEST_F(TestFixture, RunWithoutFixtures) {
    bool called = false;
    Fortest::Test test("simple", [&](void *, void *, void *) { called = true; });
    test.run(logger, assert_obj);
    EXPECT_TRUE(called);
}

/**
 * @brief Behavior: Test status reflects assertion results.
 */
TEST_F(TestFixture, GetTestStatusReflectsAssertions) {
    Fortest::Test passing_test("pass", [&](void *, void *, void *) {
        assert_obj.assert_true(true);
    });
    Fortest::Test failing_test("fail", [&](void *, void *, void *) {
        assert_obj.assert_true(false);
    });

    passing_test.run(logger, assert_obj);
    failing_test.run(logger, assert_obj);

    EXPECT_EQ(passing_test.get_status(), Fortest::Test::Status::PASS);
    EXPECT_EQ(failing_test.get_status(), Fortest::Test::Status::FAIL);
}

/**
 * @brief Behavior: Fixture setup runs before the test and teardown after.
 */
TEST_F(TestFixture, RunCallsSetupAndTeardown) {
    bool setup_called = false;
    bool teardown_called = false;
    int *arg = new int(5);

    auto fixture = std::make_shared<Fortest::Fixture<void>>(
        [&](void *) { setup_called = true; },
        [&](void *) { teardown_called = true; },
        arg,
        Fortest::Scope::Test
    );

    Fortest::Test test("with fixture", [&](void *a, void *, void *) {
        EXPECT_EQ(a, arg);
    });
    test.add_fixture(fixture);
    test.run(logger, assert_obj);

    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);
    delete arg;
}

/**
 * @brief Behavior: Teardown is called even if the test throws.
 */
TEST_F(TestFixture, RunTestFunctionThrowsStillCallsTeardown) {
    bool teardown_called = false;
    int *arg = new int(42);

    auto fixture = std::make_shared<Fortest::Fixture<void>>(
        nullptr,
        [&](void *) { teardown_called = true; },
        arg,
        Fortest::Scope::Test
    );

    Fortest::Test test("throws", [&](void *, void *, void *) {
        throw std::runtime_error("boom");
    });
    test.add_fixture(fixture);

    EXPECT_THROW(test.run(logger, assert_obj), std::runtime_error);
    EXPECT_TRUE(teardown_called);
    delete arg;
}

/**
 * @brief Behavior: Running with only a test fixture passes correct args.
 */
TEST_F(TestFixture, RunWithOnlyTestFixture) {
    int *arg = new int(42);
    bool setup_called = false;
    bool teardown_called = false;

    auto fixture = std::make_shared<Fortest::Fixture<void>>(
        [&](void *) { setup_called = true; },
        [&](void *) { teardown_called = true; },
        arg,
        Fortest::Scope::Test
    );

    Fortest::Test test("only-test", [&](void *a, void *, void *) {
        EXPECT_EQ(a, arg);
    });
    test.add_fixture(fixture);
    test.run(logger, assert_obj);

    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);
    delete arg;
}

/**
 * @brief Behavior: Running with only a suite fixture passes correct args.
 */
TEST_F(TestFixture, RunWithOnlySuiteFixture) {
    int *arg = new int(7);
    auto suite_fixture = std::make_shared<Fortest::Fixture<void>>(
        nullptr, nullptr, arg, Fortest::Scope::Suite
    );

    Fortest::Test test("only-suite", [&](void *, void *b, void *) {
        EXPECT_EQ(b, arg);
    });
    test.add_fixture(suite_fixture);
    test.run(logger, assert_obj);

    delete arg;
}

/**
 * @brief Behavior: Setup runs before test body and teardown after.
 */
TEST_F(TestFixture, SetupRunsBeforeTestAndTeardownAfter) {
    bool order_ok = false;
    int *arg = new int(1);

    auto fixture = std::make_shared<Fortest::Fixture<void>>(
        [&](void *a) { *static_cast<int *>(a) = 2; },
        [&](void *a) {
            if (*static_cast<int *>(a) == 3) order_ok = true;
        },
        arg,
        Fortest::Scope::Test
    );

    Fortest::Test test("order", [&](void *a, void *, void *) {
        EXPECT_EQ(*static_cast<int *>(a), 2); // setup ran
        *static_cast<int *>(a) = 3;
    });
    test.add_fixture(fixture);
    test.run(logger, assert_obj);

    EXPECT_TRUE(order_ok);
    delete arg;
}

/**
 * @brief Behavior: Adding multiple fixtures of the same scope overrides the previous one.
 */
TEST_F(TestFixture, MultipleAddFixtureOverridesPrevious) {
    int *arg1 = new int(5);
    int *arg2 = new int(10);

    auto f1 = std::make_shared<Fortest::Fixture<void>>(
        nullptr, nullptr, arg1, Fortest::Scope::Suite
    );
    auto f2 = std::make_shared<Fortest::Fixture<void>>(
        nullptr, nullptr, arg2, Fortest::Scope::Suite
    );

    Fortest::Test test("override", [&](void *, void *b, void *) {
        EXPECT_EQ(b, arg2); // last one should win
    });
    test.add_fixture(f1);
    test.add_fixture(f2);
    test.run(logger, assert_obj);

    delete arg1;
    delete arg2;
}
