#include "fortran_fixture_extractor.hpp"
#include <gtest/gtest.h>
#include <string>

/**
 * @brief Test fixture for FortranFixtureExtractor behavior.
 */
class FortranFixtureExtractorTest : public ::testing::Test {
protected:
    std::vector<Fortest::FixtureInfo> extract(const std::string &code) {
        Fortest::FortranFixtureExtractor extractor(code);
        return extractor.extract();
    }
};

// -----------------------------------------------------------------------------
// Basic extraction
// -----------------------------------------------------------------------------

/**
 * @test Behavior: Extracts suite fixture with correct setup, teardown, and scope.
 */
TEST_F(FortranFixtureExtractorTest, ExtractsSuiteFixture) {
    std::string code = R"(
       call session%register_fixture( &
           setup    = setup_suite_fixture, &
           teardown = teardown_suite_fixture, &
           args     = suite_ptr, &
           scope    = "suite", &
           test_suite_name = "math_ops")
    )";

    auto fixtures = extract(code);
    ASSERT_EQ(fixtures.size(), 1);

    EXPECT_EQ(fixtures[0].type_name, "suite_fixture_t");
    EXPECT_EQ(fixtures[0].scope, "suite");
    EXPECT_EQ(fixtures[0].setup, "setup_suite_fixture");
    EXPECT_EQ(fixtures[0].teardown, "teardown_suite_fixture");
}

/**
 * @test Behavior: Extracts test fixture with correct setup, teardown, and scope.
 */
TEST_F(FortranFixtureExtractorTest, ExtractsTestFixture) {
    std::string code = R"(
       call session%register_fixture( &
           setup    = setup_test_fixture, &
           teardown = teardown_test_fixture, &
           args     = test_ptr, &
           scope    = "test", &
           test_suite_name = "math_ops")
    )";

    auto fixtures = extract(code);
    ASSERT_EQ(fixtures.size(), 1);

    EXPECT_EQ(fixtures[0].type_name, "test_fixture_t");
    EXPECT_EQ(fixtures[0].scope, "test");
    EXPECT_EQ(fixtures[0].setup, "setup_test_fixture");
    EXPECT_EQ(fixtures[0].teardown, "teardown_test_fixture");
}

/**
 * @test Behavior: Extracts multiple fixtures from the same driver.
 */
TEST_F(FortranFixtureExtractorTest, ExtractsMultipleFixtures) {
    std::string code = R"(
       call session%register_fixture(setup=setup_suite_fixture, teardown=teardown_suite_fixture, scope="suite")
       call session%register_fixture(setup=setup_test_fixture, teardown=teardown_test_fixture, scope="test")
    )";

    auto fixtures = extract(code);
    ASSERT_EQ(fixtures.size(), 2);

    EXPECT_EQ(fixtures[0].type_name, "suite_fixture_t");
    EXPECT_EQ(fixtures[1].type_name, "test_fixture_t");
}

/**
 * @test Behavior: Scope is case-insensitive and normalized to lowercase.
 */
TEST_F(FortranFixtureExtractorTest, NormalizesScopeToLowercase) {
    std::string code = R"(
       call session%register_fixture(setup=setup_suite_fixture, teardown=teardown_suite_fixture, scope="SuItE")
    )";

    auto fixtures = extract(code);
    ASSERT_EQ(fixtures.size(), 1);
    EXPECT_EQ(fixtures[0].scope, "suite");
}

/**
 * @test Behavior: Non-matching code returns an empty vector.
 */
TEST_F(FortranFixtureExtractorTest, IgnoresNonFixtureCode) {
    std::string code = R"(
       program test_math_ops
          print *, "Hello world"
       end program test_math_ops
    )";

    auto fixtures = extract(code);
    EXPECT_TRUE(fixtures.empty());
}
