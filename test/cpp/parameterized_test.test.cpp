#include "parameterized_test.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>

using ::testing::HasSubstr;

// A simple logger for testing that writes into an ostringstream
class OStreamLogger : public Fortest::Logger {
public:
    explicit OStreamLogger(std::ostream &out) : Logger(out) {}
};

class ParameterizedTestFixture : public ::testing::Test {
protected:
    std::ostringstream buffer;
    std::shared_ptr<OStreamLogger> logger;
    Fortest::Assert<OStreamLogger> assert_obj{static_cast<std::ostream&>(buffer)};

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
 * @brief Behavior: A parameterized test stores and returns its name.
 */
TEST_F(ParameterizedTestFixture, ConstructorSetsName) {
    const std::string test_name = "param-test";
    const Fortest::ParameterizedTest test(
        test_name,
        Fortest::ParameterizedTestFunction(),
        {0, 1}
    );
    EXPECT_EQ(test_name, test.get_name());
}

/**
 * @brief Behavior: A parameterized test runs once per parameter index.
 */
TEST_F(ParameterizedTestFixture, RunsOncePerParameter) {
    std::vector<int> called_indices;
    Fortest::ParameterizedTest test(
        "multi-run",
        [&](void *, void *, void *, int idx) { called_indices.push_back(idx); },
        {0, 1, 2}
    );

    test.run(logger, assert_obj);

    EXPECT_EQ(called_indices.size(), 3);
    EXPECT_EQ(called_indices[0], 0);
    EXPECT_EQ(called_indices[1], 1);
    EXPECT_EQ(called_indices[2], 2);
}

/**
 * @brief Behavior: Status reflects assertions for each parameter index.
 */
TEST_F(ParameterizedTestFixture, StatusReflectsAssertions) {
    Fortest::ParameterizedTest test(
        "pass-fail",
        [&](void *, void *, void *, int idx) {
            assert_obj.assert_true(idx % 2 == 0); // even passes, odd fails
        },
        {0, 1}
    );

    test.run(logger, assert_obj);

    EXPECT_EQ(test.get_status(0), Fortest::ParameterizedTest::Status::PASS);
    EXPECT_EQ(test.get_status(1), Fortest::ParameterizedTest::Status::FAIL);
}

/**
 * @brief Behavior: Fixture setup runs before each parameter and teardown after.
 */
TEST_F(ParameterizedTestFixture, RunCallsSetupAndTeardown) {
    bool setup_called = false;
    bool teardown_called = false;
    int *arg = new int(5);

    auto fixture = std::make_shared<Fortest::Fixture<void>>(
        [&](void *) { setup_called = true; },
        [&](void *) { teardown_called = true; },
        arg,
        Fortest::Scope::Test
    );

    Fortest::ParameterizedTest test(
        "with-fixture",
        [&](void *a, void *, void *, int) { EXPECT_EQ(a, arg); },
        {0}
    );
    test.add_fixture(fixture);

    test.run(logger, assert_obj);

    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);
    delete arg;
}

/**
 * @brief Behavior: Teardown is called even if the test throws.
 */
TEST_F(ParameterizedTestFixture, RunTestFunctionThrowsStillCallsTeardown) {
    bool teardown_called = false;
    int *arg = new int(42);

    auto fixture = std::make_shared<Fortest::Fixture<void>>(
        nullptr,
        [&](void *) { teardown_called = true; },
        arg,
        Fortest::Scope::Test
    );

    Fortest::ParameterizedTest test(
        "throws",
        [&](void *, void *, void *, int) { throw std::runtime_error("boom"); },
        {0}
    );
    test.add_fixture(fixture);

    EXPECT_THROW(test.run(logger, assert_obj), std::runtime_error);
    EXPECT_TRUE(teardown_called);
    delete arg;
}

/**
 * @brief Behavior: Adding multiple fixtures of the same scope overrides the previous one.
 */
TEST_F(ParameterizedTestFixture, MultipleAddFixtureOverridesPrevious) {
    int *arg1 = new int(5);
    int *arg2 = new int(10);

    auto f1 = std::make_shared<Fortest::Fixture<void>>(
        nullptr, nullptr, arg1, Fortest::Scope::Suite
    );
    auto f2 = std::make_shared<Fortest::Fixture<void>>(
        nullptr, nullptr, arg2, Fortest::Scope::Suite
    );

    Fortest::ParameterizedTest test(
        "override",
        [&](void *, void *b, void *, int) { EXPECT_EQ(b, arg2); },
        {0}
    );
    test.add_fixture(f1);
    test.add_fixture(f2);

    test.run(logger, assert_obj);

    delete arg1;
    delete arg2;
}
