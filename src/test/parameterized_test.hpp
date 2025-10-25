#ifndef FORTEST_PARAMETERIZED_TEST_HPP
#define FORTEST_PARAMETERIZED_TEST_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include "fixture.hpp"
#include "assert.hpp"
#include "logging.hpp"

namespace Fortest {

    /// Function signature for parameterized test procedures.
    using ParameterizedTestFunction =
        std::function<void(void *, void *, void *, int)>;

    /**
     * @brief Represents a parameterized test case.
     *
     * A ParameterizedTest holds a test function, parameter values,
     * optional fixtures (test, suite, session), a name, and the result status.
     */
    class ParameterizedTest {
    public:
        /// Test execution status.
        enum class Status { PASS, FAIL, NONE };

    private:
        ParameterizedTestFunction m_test;                   //!< Test body
        std::shared_ptr<Fixture<void>> m_test_fixture;      //!< Test-level fixture
        std::shared_ptr<Fixture<void>> m_suite_fixture;     //!< Suite-level fixture
        std::shared_ptr<Fixture<void>> m_session_fixture;   //!< Session-level fixture
        std::string m_name;                                 //!< Test name
        std::vector<int> m_parameters;                      //!< Parameter indices
        std::map<int, Status> m_status_map;                 //!< Status per parameter

    public:
        /**
         * @brief Construct a parameterized test with a name, function, and parameter set.
         * @param name Test name.
         * @param test Test function (takes test, suite, session args, and index).
         * @param parameters List of parameter indices to run.
         */
        explicit ParameterizedTest(
            std::string name,
            ParameterizedTestFunction test,
            std::vector<int> parameters)
            : m_test(std::move(test)),
              m_name(std::move(name)),
              m_parameters(std::move(parameters)) {}

        /// @brief Get the name of the test.
        [[nodiscard]] const std::string &get_name() const { return m_name; }

        /**
         * @brief Attach a fixture to this test.
         * @param fixture Shared pointer to a Fixture.
         */
        void add_fixture(const std::shared_ptr<Fixture<void>> &fixture) {
            switch (fixture->get_scope()) {
                case Scope::Session:
                    m_session_fixture = fixture;
                    break;
                case Scope::Suite:
                    m_suite_fixture = fixture;
                    break;
                case Scope::Test:
                    m_test_fixture = fixture;
                    break;
            }
        }

        /**
         * @brief Run the test across all parameters with logging and assertions.
         *
         * @tparam Logger A logger type satisfying LoggerLike.
         * @param logger Shared pointer to a logger.
         * @param assert Assertion manager used to track results.
         */
        template <LoggerLike TestLoggerType = Logger, LoggerLike AssertLoggerType = TestLoggerType>
        void run(const std::shared_ptr<TestLoggerType> &logger, Assert<AssertLoggerType> &assert) {
            void *test_args = nullptr;
            void *suite_args = nullptr;
            void *session_args = nullptr;

            if (m_suite_fixture) {
                suite_args = m_suite_fixture->get_args();
            }
            if (m_session_fixture) {
                session_args = m_session_fixture->get_args();
            }

            for (int idx : m_parameters) {
                if (m_test_fixture) {
                    test_args = m_test_fixture->get_args();
                    m_test_fixture->setup();
                }

                assert.reset();

                std::string variation_name = m_name + " [param=" + std::to_string(idx) + "]";
                std::string border = "\n" + std::string(40, '=');

                logger->log("Running parameterized test: " + variation_name, "INFO", border);

                try {
                    m_test(test_args, suite_args, session_args, idx);

                    m_status_map[idx] =
                        (assert.get_num_failed() == 0)
                        ? Status::PASS
                        : Status::FAIL;

                    if (m_status_map[idx] == Status::PASS) {
                        logger->log("Test passed: " + variation_name, "PASS");
                    } else {
                        logger->log("Test failed: " + variation_name, "FAIL");
                    }

                    if (m_test_fixture) {
                        m_test_fixture->teardown();
                    }
                } catch (...) {
                    if (m_test_fixture) {
                        m_test_fixture->teardown();
                    }
                    m_status_map[idx] = Status::FAIL;
                    logger->log("Test threw exception: " + variation_name, "FAIL");
                    throw;
                }
            }
        }

        /// @brief Get the status for a specific parameter index.
        [[nodiscard]] Status get_status(int idx) const {
            auto it = m_status_map.find(idx);
            return (it != m_status_map.end()) ? it->second : Status::NONE;
        }

        /// @brief Get all parameter indices.
        [[nodiscard]] const std::vector<int>& get_parameters() const {
            return m_parameters;
        }
    };

} // namespace Fortest

#endif // FORTEST_PARAMETERIZED_TEST_HPP
