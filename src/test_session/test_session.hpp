#ifndef FORTEST_TEST_SESSION_HPP
#define FORTEST_TEST_SESSION_HPP

#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include "test_suite.hpp"
#include "fixture.hpp"

namespace Fortest {

    /**
     * @brief Represents a session of test suites.
     *
     * @details
     * A TestSession organizes multiple test suites, manages
     * optional session-level fixtures, and coordinates running all
     * registered tests with a provided logger and assertion engine.
     *
     * @tparam Logger A logger type satisfying LoggerLike.
     */
    template <LoggerLike Logger>
    class TestSession {
        Assert<AssertLogger> &m_assert;  //!< Assertion engine for all tests
        std::map<std::string,
            std::unique_ptr<TestSuite<Logger>>> m_suites; //!< Registered test suites
        std::shared_ptr<Fixture<void>> m_session_fixture; //!< Optional session-level fixture

    public:
        /// @brief Construct a TestSession with a reference to the assertion engine.
        explicit TestSession(Assert<AssertLogger> &assert) : m_assert(assert) {}

        /**
         * @brief Add a new test suite to the session.
         * @param name Name of the test suite.
         * @throws std::runtime_error if a suite with the same name already exists.
         * @return Reference to the created TestSuite.
         */
        TestSuite<Logger> &add_test_suite(const std::string &name) {
            if (m_suites.find(name) != m_suites.end()) {
                throw std::runtime_error(
                    "Test suite with name '" + name +
                    "' already exists in session."
                );
            }
            auto suite = std::make_unique<TestSuite<Logger>>(name, m_assert);
            auto &ref = *suite;
            m_suites[name] = std::move(suite);
            if (m_session_fixture) {
                m_suites[name]->add_fixture(*m_session_fixture);
            }
            return ref;
        }

        /**
         * @brief Add a session-level fixture.
         * @param fixture Fixture with Scope::Session.
         * @throws std::runtime_error if fixture does not have session scope.
         */
        void add_fixture(const Fixture<void> &fixture) {
            if (fixture.get_scope() != Scope::Session) {
                throw std::runtime_error(
                    "Fixture without suite name must have session scope."
                );
            }
            m_session_fixture = std::make_shared<Fixture<void>>(fixture);
            for (auto &[name, suite] : m_suites) {
                suite->add_fixture(fixture);
            }
        }

        /**
         * @brief Add a fixture to a specific suite.
         * @param suite_name Name of the suite.
         * @param fixture Fixture with Scope::Test or Scope::Suite.
         * @throws std::runtime_error if fixture scope is invalid
         *         or suite does not exist.
         */
        void add_fixture(
            const std::string &suite_name,
            const Fixture<void> &fixture
        ) {
            if (fixture.get_scope() != Scope::Test &&
                fixture.get_scope() != Scope::Suite) {
                throw std::runtime_error(
                    "Fixture for suite '" + suite_name +
                    "' must have test or suite scope."
                );
            }
            auto it = m_suites.find(suite_name);
            if (it == m_suites.end()) {
                throw std::runtime_error(
                    "Suite '" + suite_name +
                    "' does not exist in session."
                );
            }
            it->second->add_fixture(fixture);
        }

        /**
         * @brief Add a test to a suite.
         * @param suite_name Name of the suite.
         * @param test_name Name of the test.
         * @param func Test function (void(*)(void*, void*, void*)).
         * @throws std::runtime_error if suite does not exist.
         */
        void add_test(
            const std::string &suite_name,
            const std::string &test_name,
            TestFunction func
        ) {
            auto it = m_suites.find(suite_name);
            if (it == m_suites.end()) {
                throw std::runtime_error(
                    "Suite '" + suite_name +
                    "' does not exist in session."
                );
            }
            it->second->add_test(test_name, std::move(func));
        }

        /**
         * @brief Run all test suites in the session.
         * @param logger Shared pointer to logger.
         */
        void run(const std::shared_ptr<Logger> &logger) {
            logger->log("Starting test session: ", "INFO");

            if (m_session_fixture) {
                m_session_fixture->setup();
            }

            for (auto &[name, suite] : m_suites) {
                logger->log("Running test suite: " + name, "INFO");

                // Inject session fixture into each suite
                suite->add_fixture(
                    Fixture<void>(
                        nullptr, nullptr,
                        m_session_fixture
                            ? m_session_fixture->get_args()
                            : nullptr,
                        Scope::Session
                    )
                );

                suite->run(logger);
            }

            if (m_session_fixture) {
                m_session_fixture->teardown();
            }

            logger->log("Finished test session: ", "INFO");
        }

        /**
         * @brief Get the status of all tests in a suite.
         * @param suite_name Name of the suite.
         * @return Map of test names to statuses.
         * @throws std::runtime_error if suite does not exist.
         */
        [[nodiscard]] std::map<std::string, Test::Status>
        get_test_suite_status(const std::string &suite_name) const {
            auto it = m_suites.find(suite_name);
            if (it == m_suites.end()) {
                throw std::runtime_error(
                    "Suite '" + suite_name +
                    "' does not exist in session."
                );
            }
            return it->second->get_statuses();
        }
    };

} // namespace Fortest

#endif // FORTEST_TEST_SESSION_HPP
