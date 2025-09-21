#ifndef FORTEST_GLOBAL_TEST_SESSION_HPP
#define FORTEST_GLOBAL_TEST_SESSION_HPP

#include <g_logging.hpp>

#include "test_session.hpp"
#include "assert.hpp"
#include "g_assert.hpp"

namespace Fortest {

    /// @brief Singleton providing global access to a TestSession.
    ///
    /// Wraps a single instance of `TestSession<Logger>` that is
    /// initialized with the global assert instance. Ensures that
    /// tests can share the same session without explicitly passing
    /// references around.
    class GlobalTestSession {
    public:
        /// @brief Access the global test session instance.
        ///
        /// The session is lazily constructed on first call and
        /// returned by reference on subsequent calls.
        ///
        /// @return Reference to the global `TestSession<Logger>`.
        static TestSession<Logger> &instance() {
            static TestSession session(*GlobalAssert::instance());
            return session;
        }

    private:
        /// @brief Hidden default constructor.
        GlobalTestSession() = default;

        /// @brief Hidden destructor.
        ~GlobalTestSession() = default;

        /// @brief Deleted copy constructor.
        GlobalTestSession(const GlobalTestSession &) = delete;

        /// @brief Deleted copy assignment operator.
        GlobalTestSession &operator=(const GlobalTestSession &) = delete;
    };
}

#endif // FORTEST_GLOBAL_TEST_SESSION_HPP
