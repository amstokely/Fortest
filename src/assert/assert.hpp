#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <memory>
#include <string>
#include <sstream>
#include <concepts>
#include <ranges>
#include "logging.hpp"

namespace Fortest {
    /**
     * @brief Assertion utility class with logging support.
     *
     * This class provides test-style assertion functions (equality,
     * inequality, truth, falsehood) that track the number of passed and
     * failed assertions. Each assertion can optionally use a logger
     * that satisfies the LoggerLike concept, allowing results to be
     * logged consistently.
     *
     * Features:
     * - Tracks pass/fail counters
     * - Provides type-safe equality/inequality assertions
     * - Handles boolean assertions
     * - Converts arbitrary values to string representations for logging
     * - Compatible with custom loggers
     *
     * @tparam Logger A type that models the LoggerLike concept.
     */
    template<LoggerLike Logger> class Assert {
        int m_num_passed{}; ///< Number of assertions that have passed
        int m_num_failed{}; ///< Number of assertions that have failed

        /**
         * @brief Convert a value to a string for reporting/logging.
         *
         * Uses several strategies depending on the type:
         * - `std::string` or convertible to `std::string_view`: direct conversion
         * - Arithmetic types: `std::to_string`
         * - Ranges (e.g. `std::vector`): element-wise formatting
         * - Types with `operator<<`: streamed into an `ostringstream`
         * - Otherwise: "<unprintable>"
         *
         * @tparam Value The type of the value.
         * @param value  The value to convert.
         * @return std::string A string representation of the value.
         */
        template<typename Value> static std::string to_string_repr(
            const Value &value
        ) {
            if constexpr (std::convertible_to<Value,
                std::string_view>) {
                return std::string{std::string_view(value)};
            } else if constexpr (
                std::integral<Value> || std::floating_point<Value>) {
                return std::to_string(value);
            } else if constexpr (std::ranges::range<Value> &&
                                 !std::convertible_to<Value,
                                     std::string_view>) {
                std::ostringstream oss;
                oss << "[";
                bool first = true;
                for (auto &&elem: value) {
                    if (!first) oss << ", ";
                    oss << to_string_repr(elem);
                    first = false;
                }
                oss << "]";
                return oss.str();
            } else if constexpr (requires(
            std::ostream &os, const Value &v
        )
                {
                    os << v;
                }) {
                std::ostringstream oss;
                oss << value;
                return oss.str();
            } else {
                return "<unprintable>";
            }
        }

    public:
        /**
         * @brief Assert that two values are equal.
         *
         * For floating-point values, supports absolute and relative tolerance.
         * For other types, uses exact equality.
         *
         * @tparam T Type of the values (must be equality-comparable).
         * @param expected The expected value.
         * @param actual   The actual value.
         * @param logger   A logger instance.
         * @param abs_tol  Absolute tolerance (for floating-point).
         * @param rel_tol  Relative tolerance (for floating-point).
         */
        template<typename T>
        void assert_equal(
            const T &expected,
            const T &actual,
            const std::shared_ptr<Logger> &logger,
            std::conditional_t<std::is_floating_point_v<T>, T, double> abs_tol = 0,
            std::conditional_t<std::is_floating_point_v<T>, T, double> rel_tol = 0
        ) {
            bool pass = false;

            if constexpr (std::is_floating_point_v<T>) {
                auto diff = std::abs(expected - actual);
                pass = (diff <= abs_tol) ||
                       (diff <= rel_tol * std::max(std::abs(expected), std::abs(actual)));
            } else {
                pass = (expected == actual);
            }

            if (pass) {
                ++m_num_passed;
            } else {
                ++m_num_failed;
            }
        }

        /**
         * @brief Assert that two values are not equal.
         *
         * For floating-point values, supports absolute and relative tolerance.
         * For other types, uses exact inequality.
         *
         * @tparam T Type of the values (must be equality-comparable).
         * @param expected The expected value.
         * @param actual   The actual value.
         * @param logger   A logger instance.
         * @param abs_tol  Absolute tolerance (for floating-point).
         * @param rel_tol  Relative tolerance (for floating-point).
         */
        template<typename T>
        void assert_not_equal(
            const T &expected,
            const T &actual,
            const std::shared_ptr<Logger> &logger,
            std::conditional_t<std::is_floating_point_v<T>, T, double> abs_tol = 0,
            std::conditional_t<std::is_floating_point_v<T>, T, double> rel_tol = 0
        ) {
            bool pass = false;

            if constexpr (std::is_floating_point_v<T>) {
                auto diff = std::abs(expected - actual);
                bool nearly_equal = (diff <= abs_tol) ||
                                    (diff <= rel_tol * std::max(std::abs(expected), std::abs(actual)));
                pass = !nearly_equal;
            } else {
                pass = (expected != actual);
            }

            if (pass) {
                ++m_num_passed;
            } else {
                ++m_num_failed;
            }
        }


        /**
         * @brief Assert that a condition is true.
         *
         * Increments pass/fail counters accordingly.
         *
         * @param condition The condition to check.
         * @param logger    A logger instance (optional).
         */
        void assert_true(
            bool condition, const std::shared_ptr<Logger> &logger
        ) {
            if (condition) {
                ++m_num_passed;
            } else {
                ++m_num_failed;
            }
        }

        /**
         * @brief Assert that a condition is false.
         *
         * Increments pass/fail counters accordingly.
         *
         * @param condition The condition to check.
         * @param logger    A logger instance (optional).
         */
        void assert_false(
            bool condition, const std::shared_ptr<Logger> &logger
        ) {
            if (!condition) {
                ++m_num_passed;
            } else {
                ++m_num_failed;
            }
        }

        /**
         * @brief Get the number of passed assertions.
         * @return int The number of assertions that passed.
         */
        int get_num_passed() const { return m_num_passed; }

        /**
         * @brief Get the number of failed assertions.
         * @return int The number of assertions that failed.
         */
        int get_num_failed() const { return m_num_failed; }

        /**
         * @brief Reset assertion counters to zero.
         */
        void reset() { m_num_passed = m_num_failed = 0; }
    };
}

#endif // ASSERT_HPP
