#ifndef FORTEST_TEST_SUITE_HPP
#define FORTEST_TEST_SUITE_HPP

#include "test.hpp"

namespace Fortest {
    /**
     * @brief Represents a collection of tests within a suite.
     *
     * @details
     * A TestSuite groups related tests, manages suite-level and
     * test-level fixtures, and tracks test statuses. Fixtures can be
     * attached at the test, suite, or session scope.
     *
     * @tparam Logger A logger type satisfying LoggerLike.
     */
    template<LoggerLike Logger> class TestSuite {
        std::string m_name; //!< Name of the test suite
        std::map<std::string, Test> m_tests;
        //!< Map of test names to test objects
        std::shared_ptr<Fixture<void> > m_test_fixture;
        //!< Test-level fixture
        std::shared_ptr<Fixture<void> > m_suite_fixture;
        //!< Suite-level fixture
        std::shared_ptr<Fixture<void> > m_session_fixture;
        //!< Session-level fixture
        Assert<AssertLogger> &m_assert; //!< Assertion engine
        std::map<std::string, Test::Status> m_statuses;
        //!< Status of each test

    public:
        /**
         * @brief Construct a TestSuite with a name and assertion engine.
         * @param name   Suite name.
         * @param assert Reference to the assertion manager.
         */
        TestSuite(std::string name, Assert<AssertLogger> &assert)
            : m_name(std::move(name)), m_assert(assert) {
        }

        /**
         * @brief Add a fixture to the suite.
         * @param fixture Fixture with scope Test, Suite, or Session.
         */
        void add_fixture(const Fixture<void> &fixture) {
            switch (fixture.get_scope()) {
                case Scope::Session:
                    if (!m_session_fixture) {
                        m_session_fixture = std::make_shared<Fixture<
                            void> >(fixture);
                    }
                break;
                case Scope::Suite:
                    if (!m_suite_fixture) {
                        m_suite_fixture = std::make_shared<Fixture<
                            void> >(fixture);
                    }
                    break;
                case Scope::Test:
                    if (!m_test_fixture) {
                        m_test_fixture = std::make_shared<Fixture<
                            void> >(fixture);
                    }
                    break;
            }
            for (auto &test: m_tests | std::views::values) {
                test.add_fixture(std::make_shared<Fixture<void>>(fixture));
            }
        }

        /**
         * @brief Add a test to the suite.
         *
         * @param test_name Name of the test.
         * @param func      Test function.
         */
        void add_test(const std::string &test_name, TestFunction func) {
            Test test(test_name, std::move(func));

            if (m_test_fixture) {
                test.add_fixture(m_test_fixture);
            }
            if (m_suite_fixture) {
                test.add_fixture(m_suite_fixture);
            }
            if (m_session_fixture) {
                test.add_fixture(m_session_fixture);
            }

            m_statuses[test_name] = test.get_status();
            m_tests.emplace(test_name, std::move(test));
        }

        /// @brief Get the name of this test suite.
        [[nodiscard]] const std::string &get_name() const {
            return m_name;
        }

        /// @brief Get a map of test names to their statuses.
        [[nodiscard]] const std::map<std::string, Test::Status> &
        get_statuses() const {
            return m_statuses;
        }

        /**
         * @brief Run all tests in the suite.
         *
         * @param logger Shared pointer to a logger.
         */
        void run(const std::shared_ptr<Logger> &logger) {
            if (m_suite_fixture) {
                m_suite_fixture->setup();
            }

            for (auto &[test_name, test]: m_tests) {
                logger->log("Running test: " + test_name, "INFO");
                test.run(logger, m_assert);
                m_statuses[test_name] = test.get_status();

                if (test.get_status() == Test::Status::PASS) {
                    logger->log("Test passed: " + test_name, "PASS");
                } else {
                    logger->log("Test failed: " + test_name, "FAIL");
                }
            }

            if (m_suite_fixture) {
                m_suite_fixture->teardown();
            }
        }
    };
} // namespace Fortest

#endif // FORTEST_TEST_SUITE_HPP
