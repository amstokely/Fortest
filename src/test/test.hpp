#ifndef FORTEST_TEST_HPP
#define FORTEST_TEST_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include "fixture.hpp"
#include "assert.hpp"
#include "db.hpp"
#include "logging.hpp"

namespace Fortest {
    /// Function signature for test procedures (test, suite, session args).
    using TestFunction = std::function<void(void *, void *, void *)>;

    /**
     * @brief Represents a single test case.
     *
     * A Test holds a test function, optional fixtures (test, suite,
     * session), a name, and the result status.
     */
    class Test {
    public:
        /// Test execution status.
        enum class Status { PASS, FAIL, NONE };

    private:
        TestFunction m_test; //!< Test body
        std::shared_ptr<Fixture<void> > m_test_fixture; //!< Test-level fixture
        std::shared_ptr<Fixture<void> > m_suite_fixture;
        //!< Suite-level fixture
        std::shared_ptr<Fixture<void> > m_session_fixture;
        //!< Session-level fixture
        std::string m_name; //!< Test name
        Status m_status = Status::NONE; //!< Test result status

    public:
        /**
         * @brief Construct a test with a name and function.
         * @param name Test name.
         * @param test Test function (takes test, suite, session args).
         */
        explicit Test(std::string name, TestFunction test)
            : m_test(std::move(test)), m_name(std::move(name)) {
        }

        /// @brief Get the name of the test.
        [[nodiscard]] const std::string &get_name() const { return m_name; }

        /**
         * @brief Attach a fixture to this test.
         * @param fixture Shared pointer to a Fixture.
         */
        void add_fixture(const std::shared_ptr<Fixture<void> > &fixture) {
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
         * @brief Run the test with logging and assertions.
         *
         * @tparam Logger A logger type satisfying LoggerLike.
         * @param logger Shared pointer to a logger.
         * @param assert Assertion manager used to track results.
         */
        template<LoggerLike TestLoggerType = Logger, LoggerLike AssertLoggerType
            = TestLoggerType>
        void run(const std::shared_ptr<TestLoggerType> &logger,
                 Assert<AssertLoggerType> &assert,
                 const std::optional<SqliteDb> &db = std::nullopt) {
            void *test_args = nullptr;
            void *suite_args = nullptr;
            void *session_args = nullptr;
            if (m_test_fixture) {
                test_args = m_test_fixture->get_args();
                m_test_fixture->setup();
            }
            if (m_suite_fixture) {
                suite_args = m_suite_fixture->get_args();
            }
            if (m_session_fixture) {
                session_args = m_session_fixture->get_args();
            }

            assert.reset();

            auto args = std::make_tuple(test_args, suite_args, session_args);

            try {
                std::apply(m_test, args);

                m_status = (assert.get_num_failed() == 0)
                               ? Status::PASS
                               : Status::FAIL;
                if (db.has_value()) {
                    SqliteStmt stmt(db.value().get(),
                                    "INSERT INTO test_results (test_name, status, duration_ms) "
                                    "VALUES (?, ?, ?);");
                    std::string status_str = (m_status == Status::PASS)
                                                 ? "PASS"
                                                 : "FAIL";
                    sqlite3_bind_text(stmt.get(), 1, m_name.c_str(), -1,
                                      SQLITE_TRANSIENT);
                    sqlite3_bind_text(stmt.get(), 2, status_str.c_str(), -1,
                                      SQLITE_TRANSIENT);
                    sqlite3_bind_int(stmt.get(), 3, 0);
                    stmt.step();
                }
                if (m_test_fixture) {
                    m_test_fixture->teardown();
                }
            } catch (...) {
                if (m_test_fixture) {
                    m_test_fixture->teardown();
                }
                m_status = Status::FAIL;
                throw;
            }
        }

        /// @brief Get the current status of the test.
        [[nodiscard]] Status get_status() const { return m_status; }
    };
} // namespace Fortest

#endif // FORTEST_TEST_HPP
