#ifndef FORTEST_FIXTURE_HPP
#define FORTEST_FIXTURE_HPP
#include <algorithm>
#include <functional>

namespace Fortest {
    /// @brief Generic function type for setup/teardown with untyped arguments.
    using FixtureFunction = std::function<void(void *)>;

    /// @brief Alias for untyped fixture arguments.
    using Args = void *;

    /// @brief Fixture lifetime scope.
    enum class Scope {
        Test,   ///< Reset before every test.
        Suite,  ///< Reset before each test suite.
        Session ///< Reset once for the entire test session.
    };

    /// @brief Represents a test fixture with setup/teardown functions, arguments, and scope.
    ///
    /// @tparam T Argument type passed to setup/teardown (defaults to void).
    template <typename T = void>
    class Fixture {
        using FixtureFunction = std::function<void(T*)>;

        FixtureFunction m_setup{};    ///< Setup function pointer.
        FixtureFunction m_teardown{}; ///< Teardown function pointer.
        T* m_args{};                  ///< Pointer to fixture arguments.
        Scope m_scope{};              ///< Fixture scope.
    public:
        /// @brief Construct a new Fixture.
        ///
        /// @param setup Function to initialize the fixture.
        /// @param teardown Function to clean up the fixture.
        /// @param args Pointer to fixture arguments (user-defined).
        /// @param scope Scope of the fixture (test, suite, or session).
        Fixture(FixtureFunction setup,
                FixtureFunction teardown,
                T* args,
                Scope scope)
            : m_setup(std::move(setup)),
              m_teardown(std::move(teardown)),
              m_args(args),
              m_scope(scope) {}

        /// @brief Run the setup function if defined.
        void setup() const { if (m_setup) m_setup(m_args); }

        /// @brief Run the teardown function if defined.
        void teardown() const { if (m_teardown) m_teardown(m_args); }

        /// @name Copy/Move Semantics
        /// @{
        Fixture(const Fixture&) = default;
        Fixture(Fixture&&) noexcept = default;
        Fixture& operator=(const Fixture&) = default;
        Fixture& operator=(Fixture&&) noexcept = default;
        /// @}

        /// @brief Get the fixture scope.
        /// @return The scope of this fixture.
        [[nodiscard]] Scope get_scope() const noexcept { return m_scope; }

        /// @brief Get the fixture arguments.
        /// @return Pointer to the fixture arguments.
        [[nodiscard]] T* get_args() const noexcept { return m_args; }
    };

} // namespace Fortest

#endif //FORTEST_FIXTURE_HPP
