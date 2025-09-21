#include "fixture.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

/**
 * @brief Test fixture for Fortest::Fixture<void>.
 * Provides shared state across individual test cases.
 */
class FixtureTest : public ::testing::Test {
protected:
    bool setup_called{};
    bool teardown_called{};

    void SetUp() override {
        setup_called = false;
        teardown_called = false;
    }
};

/**
 * @test Behavior: setup() invokes the setup function.
 */
TEST_F(FixtureTest, SetupInvokesFunction) {
    Fortest::Fixture<void> f(
        [&](void*) { setup_called = true; },
        nullptr,
        nullptr,
        Fortest::Scope::Test
    );

    f.setup();
    EXPECT_TRUE(setup_called);
}

/**
 * @test Behavior: teardown() invokes the teardown function.
 */
TEST_F(FixtureTest, TeardownInvokesFunction) {
    Fortest::Fixture<void> f(
        nullptr,
        [&](void*) { teardown_called = true; },
        nullptr,
        Fortest::Scope::Test
    );

    f.teardown();
    EXPECT_TRUE(teardown_called);
}

/**
 * @test Behavior: setup() and teardown() can both be invoked.
 */
TEST_F(FixtureTest, SetupAndTeardownBothInvoked) {
    Fortest::Fixture<void> f(
        [&](void*) { setup_called = true; },
        [&](void*) { teardown_called = true; },
        nullptr,
        Fortest::Scope::Test
    );

    f.setup();
    f.teardown();

    EXPECT_TRUE(setup_called);
    EXPECT_TRUE(teardown_called);
}

/**
 * @test Behavior: Arguments are passed to setup() and teardown().
 */
TEST_F(FixtureTest, PassesArgumentsToFunctions) {
    int arg_value = 0;

    auto setup = [&](void* a) { *static_cast<int*>(a) = 10; };
    auto teardown = [&](void* a) { *static_cast<int*>(a) = 20; };

    Fortest::Fixture<void> f(setup, teardown, &arg_value, Fortest::Scope::Test);

    f.setup();
    EXPECT_EQ(arg_value, 10);

    f.teardown();
    EXPECT_EQ(arg_value, 20);
}

/**
 * @test Behavior: The args pointer is stored and retrievable.
 */
TEST_F(FixtureTest, StoresAndReturnsArgs) {
    int arg = 42;
    Fortest::Fixture<void> f(nullptr, nullptr, &arg, Fortest::Scope::Test);

    EXPECT_EQ(f.get_args(), &arg);
}

/**
 * @test Behavior: setup() and teardown() may be null and do nothing.
 */
TEST_F(FixtureTest, NullSetupAndTeardownDoNothing) {
    Fortest::Fixture<void> f(nullptr, nullptr, nullptr, Fortest::Scope::Test);

    EXPECT_NO_THROW({
        f.setup();
        f.teardown();
    });
}
