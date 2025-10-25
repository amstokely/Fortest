#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <memory>
#include <string>
#include <sstream>
#include <concepts>
#include <ranges>
#include "assert_logger.hpp"

namespace Fortest {
    enum class Verbosity {
        QUIET = 0,
        FAIL_ONLY = 1,
        ALL = 2
    };

    template<typename LoggerType = AssertLogger>
    class Assert {
        int m_num_passed{};
        int m_num_failed{};
        std::shared_ptr<LoggerType> m_logger; ///< internal logger

        template<typename Value>
        static std::string to_string_repr(const Value &value) {
            if constexpr (std::convertible_to<Value, std::string_view>) {
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
            } else if constexpr (requires(std::ostream &os, const Value &v) { os << v; }) {
                std::ostringstream oss;
                oss << value;
                return oss.str();
            } else {
                return "<unprintable>";
            }
        }

    public:
        template<typename... Args>
        explicit Assert(Args&&... args)
            : m_logger(std::make_shared<LoggerType>(std::forward<Args>(args)...)) {}


        template<typename T>
        void assert_equal(
            const T &expected,
            const T &actual,
            std::conditional_t<std::is_floating_point_v<T>, T, double> abs_tol = 0,
            std::conditional_t<std::is_floating_point_v<T>, T, double> rel_tol = 0,
            Verbosity verbosity = Verbosity::QUIET
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
                if (verbosity == Verbosity::ALL) {
                    auto msg = "values are equal (" + to_string_repr(expected) +
                               " == " + to_string_repr(actual) + ")";
                    m_logger->log(msg, "PASS");
                }
                ++m_num_passed;
            } else {
                if (verbosity != Verbosity::QUIET) {
                    m_logger->log(
                        "values are not equal (" + to_string_repr(expected) +
                        " != " + to_string_repr(actual) + ")", "FAIL");
                }
                ++m_num_failed;
            }
        }

        template<typename T>
        void assert_not_equal(
            const T &expected,
            const T &actual,
            std::conditional_t<std::is_floating_point_v<T>, T, double> abs_tol = 0,
            std::conditional_t<std::is_floating_point_v<T>, T, double> rel_tol = 0,
            Verbosity verbosity = Verbosity::QUIET
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
                if (verbosity == Verbosity::ALL) {
                    m_logger->log(
                        "values are not equal (" + to_string_repr(expected) +
                        " != " + to_string_repr(actual) + ")", "PASS");
                }
                ++m_num_passed;
            } else {
                if (verbosity != Verbosity::QUIET) {
                    m_logger->log(
                        "values are equal (" + to_string_repr(expected) +
                        " == " + to_string_repr(actual) + ")", "FAIL");
                }
                ++m_num_failed;
            }
        }

        void assert_true(bool condition, Verbosity verbosity = Verbosity::QUIET) {
            if (condition) {
                if (verbosity == Verbosity::ALL) {
                    m_logger->log("condition is true", "PASS");
                }
                ++m_num_passed;
            } else {
                if (verbosity != Verbosity::QUIET) {
                    m_logger->log("condition is false", "FAIL");
                }
                ++m_num_failed;
            }
        }

        void assert_false(bool condition, Verbosity verbosity = Verbosity::QUIET) {
            if (!condition) {
                if (verbosity == Verbosity::ALL) {
                    m_logger->log("condition is false", "PASS");
                }
                ++m_num_passed;
            } else {
                if (verbosity != Verbosity::QUIET) {
                    m_logger->log("condition is true", "FAIL");
                }
                ++m_num_failed;
            }
        }

        int get_num_passed() const { return m_num_passed; }
        int get_num_failed() const { return m_num_failed; }
        void reset() { m_num_passed = m_num_failed = 0; }

        /// @brief Access the underlying logger.
        std::shared_ptr<LoggerType> get_logger() const { return m_logger; }
    };
} // namespace Fortest

#endif // ASSERT_HPP
