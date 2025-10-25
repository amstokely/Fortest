#ifndef G_LOGGING_HPP
#define G_LOGGING_HPP
#include "global_base.hpp"
#include "logging.hpp"

namespace Fortest {

    /// @brief Global singleton logger for Fortest.
    ///
    /// Provides a single global instance of the default
    /// `Logger` that can be shared across the framework.
    /// Inherits from both `Logger` and `SingletonBase`
    /// to combine logging functionality with a singleton
    /// access pattern.
    class GlobalLogger : public Logger, public SingletonBase<Logger> {
    private:
        /// @brief Allow SingletonBase to construct the instance.
        friend class SingletonBase<GlobalLogger>;

        /// @brief Hidden constructor for singleton use only.
        GlobalLogger() = default;
    };


    /// @brief Global singleton instance of AssertLogger.
    ///
    /// Provides a single shared `AssertLogger` that can be
    /// used across all `Assert` objects, test suites, and fixtures.
    /// This ensures consistent reporting and allows summaries to be
    /// aggregated globally.
    class GlobalAssertLogger : public AssertLogger,
                               public SingletonBase<GlobalAssertLogger> {
    private:
        /// @brief Allow SingletonBase to construct the instance.
        friend class SingletonBase<GlobalAssertLogger>;

        /// @brief Hidden constructor for singleton use only.
        GlobalAssertLogger() = default;
    };
}

#endif //G_LOGGING_HPP