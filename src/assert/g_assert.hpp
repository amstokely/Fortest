#ifndef G_ASSERT_HPP
#define G_ASSERT_HPP

#include "assert.hpp"      // Assert class
#include "global_base.hpp" // SingletonBase class

namespace Fortest {

    /// @brief Global singleton for assertions.
    ///
    /// Provides a single global instance of `Assert<Logger>`.
    /// This ensures all tests use the same assertion manager
    /// without needing to pass it explicitly. Inherits from
    /// both `Assert<Logger>` for assertion functionality and
    /// `SingletonBase` to enforce a single global instance.
    class GlobalAssert : public Assert<Logger>,
                         public SingletonBase<Assert<Logger>> {
    private:
        /// @brief Allow SingletonBase to construct the instance.
        friend class SingletonBase<Assert<Logger>>;

        /// @brief Hidden constructor for singleton use only.
        GlobalAssert() = default;
    };
}

#endif // G_ASSERT_HPP
