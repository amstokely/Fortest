#ifndef FORTEST_TEST_SUITE_HPP
#define FORTEST_TEST_SUITE_HPP

#include "test.hpp"
#include "parameterized_test.hpp"

namespace Fortest {
    /**
     * @brief Represents a collection of tests within a suite.
     *
     * @details
     * A TestSuite groups related tests, manages suite-level and
     * test-level fixtures, and tracks test statuses. Fixtures can be
     * attached at the test, suite, or session scope.
     *
     * @tparam TestLoggerType A logger type satisfying LoggerLike.
     */
    template<LoggerLike TestLoggerType, LoggerLike AssertLoggerType = TestLoggerType>
    class TestSuite {
        std::string m_name; //!< Name of the test suite
        std::map<std::string, Test> m_tests; //!< Named tests
        std::map<std::string, ParameterizedTest> m_param_tests; //!< Named parameterized tests

        std::shared_ptr<Fixture<void>> m_test_fixture;    //!< Test-level fixture
        std::shared_ptr<Fixture<void>> m_suite_fixture;   //!< Suite-level fixture
        std::shared_ptr<Fixture<void>> m_session_fixture; //!< Session-level fixture

        Assert<AssertLoggerType> &m_assert; //!< Assertion engine
        std::map<std::string, Test::Status> m_statuses; //!< Status of all tests

    public:
        TestSuite(std::string name, Assert<AssertLoggerType> &assert)
            : m_name(std::move(name)), m_assert(assert) {}

        /// @brief Add a fixture to the suite.
        void add_fixture(const Fixture<void> &fixture) {
            auto shared_fix = std::make_shared<Fixture<void>>(fixture);

            switch (fixture.get_scope()) {
                case Scope::Session:
                    if (!m_session_fixture) m_session_fixture = shared_fix;
                    break;
                case Scope::Suite:
                    if (!m_suite_fixture) m_suite_fixture = shared_fix;
                    break;
                case Scope::Test:
                    if (!m_test_fixture) m_test_fixture = shared_fix;
                    break;
            }

            for (auto &test : m_tests | std::views::values) {
                test.add_fixture(std::make_shared<Fixture<void>>(fixture));
            }
            for (auto &ptest : m_param_tests | std::views::values) {
                ptest.add_fixture(std::make_shared<Fixture<void>>(fixture));
            }
        }

        /// @brief Add a regular test to the suite.
        void add_test(const std::string &test_name, TestFunction func) {
            Test test(test_name, std::move(func));

            if (m_test_fixture)    test.add_fixture(m_test_fixture);
            if (m_suite_fixture)   test.add_fixture(m_suite_fixture);
            if (m_session_fixture) test.add_fixture(m_session_fixture);

            m_statuses[test_name] = test.get_status();
            m_tests.emplace(test_name, std::move(test));
        }

        /// @brief Add a parameterized test to the suite.
        void register_parameterized_test(const std::string &test_name,
                                         ParameterizedTestFunction func,
                                         std::vector<int> params) {
            ParameterizedTest test(test_name, std::move(func), std::move(params));

            if (m_test_fixture)    test.add_fixture(m_test_fixture);
            if (m_suite_fixture)   test.add_fixture(m_suite_fixture);
            if (m_session_fixture) test.add_fixture(m_session_fixture);

            m_param_tests.emplace(test_name, std::move(test));
        }

        [[nodiscard]] const std::string &get_name() const { return m_name; }

        /// @brief Get a map of all test names to their statuses (both regular and parameterized).
        [[nodiscard]] std::map<std::string, Test::Status> get_statuses() const {
            auto combined = m_statuses;
            for (auto &[name, ptest] : m_param_tests) {
                // If all parameter cases passed, mark PASS; if any failed, mark FAIL.
                bool any_fail = false;
                bool any_pass = false;
                for (int idx : ptest.get_parameters()) {
                    auto st = ptest.get_status(idx);
                    if (st == ParameterizedTest::Status::FAIL) any_fail = true;
                    if (st == ParameterizedTest::Status::PASS) any_pass = true;
                }
                if (any_fail) {
                    combined[name] = Test::Status::FAIL;
                } else if (any_pass) {
                    combined[name] = Test::Status::PASS;
                } else {
                    combined[name] = Test::Status::NONE;
                }
            }
            return combined;
        }

        /// @brief Run all tests and parameterized tests in the suite.
        void run(const std::shared_ptr<Logger> &logger) {
            if (m_suite_fixture) m_suite_fixture->setup();

            // Regular tests
            const auto db_opt = std::make_optional<SqliteDb>(std::move(m_name + ".sqlite"));
            db_opt.value().exec(
                 "CREATE TABLE IF NOT EXISTS test_results ("
                 "  test_name TEXT,"
                 "  status TEXT,"
                 "  duration_ms INTEGER"
                 ");");
            for (auto &[test_name, test] : m_tests) {
                std::string border = "\n" + std::string(40, '=');
                logger->log("Running test: " + test_name, "INFO", border);

                test.run(logger, m_assert, db_opt);
                m_statuses[test_name] = test.get_status();

                if (test.get_status() == Test::Status::PASS) {
                    logger->log("Test passed: " + test_name, "PASS");
                } else {
                    logger->log("Test failed: " + test_name, "FAIL");
                }
            }

            // Parameterized tests
            for (auto &[test_name, ptest] : m_param_tests) {
                std::string border = "\n" + std::string(40, '=');
                logger->log("Running parameterized test: " + test_name, "INFO", border);

                ptest.run(logger, m_assert);

                auto statuses = get_statuses();
                auto st = statuses[test_name];
                if (st == Test::Status::PASS) {
                    logger->log("Parameterized test passed: " + test_name, "PASS");
                } else if (st == Test::Status::FAIL) {
                    logger->log("Parameterized test failed: " + test_name, "FAIL");
                } else {
                    logger->log("Parameterized test not run: " + test_name, "NONE");
                }
            }

            if (m_suite_fixture) m_suite_fixture->teardown();
        }
    };
} // namespace Fortest

#endif // FORTEST_TEST_SUITE_HPP
