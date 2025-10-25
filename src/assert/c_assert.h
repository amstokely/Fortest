#ifndef C_ASSERT_HPP
#define C_ASSERT_HPP

#include "assert.hpp"
#include "g_logging.hpp"
#include "g_assert.hpp"

#include <iostream>
#include <cstdlib>

/// @file c_assert.hpp
/// @brief C bindings for the Fortest assertion framework with verbosity.
///
/// These functions provide a C-compatible interface to the Fortest
/// assertion utilities. Each function delegates to the global
/// Fortest logger and assertion engine. All exceptions are caught,
/// printed to `stderr`, and cause the program to terminate.
/// This ensures no C++ exception ever crosses the C boundary.
///
/// Verbosity levels are integers for Fortran interoperability:
/// - 0 = QUIET     (no output, even on failure)
/// - 1 = FAIL_ONLY (default, print only on failures)
/// - 2 = ALL       (print on pass and fail)

extern "C" {
inline auto fortest_logger() {
    return Fortest::GlobalAssertLogger::instance();
}

inline auto fortest_assert() {
    return Fortest::GlobalAssert::instance();
}

inline void fortest_c_assert_fatal(const char *func) noexcept {
    try {
        throw; // rethrow current exception
    } catch (const std::exception &e) {
        std::cerr << "[FORTEST FATAL] Exception in " << func
                << ": " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FORTEST FATAL] Unknown exception in "
                << func << std::endl;
    }
    std::terminate();
}

///
/// @brief Assert that a condition is true.
/// @param condition Integer treated as boolean (non-zero = true).
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_true(const int condition, const int verbosity) {
    try {
        fortest_assert()->assert_true(
            condition,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_true");
    }
}

///
/// @brief Assert that a condition is false.
/// @param condition Integer treated as boolean (zero = false).
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_false(const int condition, const int verbosity) {
    try {
        fortest_assert()->assert_false(
            condition,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_false");
    }
}

///
/// @brief Assert that two integers are equal.
/// @param expected Expected integer value.
/// @param actual   Actual integer value.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_equal_int(const int expected, const int actual, const int verbosity) {
    try {
        fortest_assert()->assert_equal(
            expected, actual,
            0.0, 0.0,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_int");
    }
}

///
/// @brief Assert that two doubles are equal within tolerances.
/// @param expected Expected double value.
/// @param actual   Actual double value.
/// @param abs_tol  Absolute tolerance.
/// @param rel_tol  Relative tolerance.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_equal_double(const double expected, const double actual,
                           const double abs_tol, const double rel_tol,
                           const int verbosity) {
    try {
        fortest_assert()->assert_equal(
            expected, actual,
            abs_tol, rel_tol,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_double");
    }
}

///
/// @brief Assert that two floats are equal within tolerances.
/// @param expected Expected float value.
/// @param actual   Actual float value.
/// @param abs_tol  Absolute tolerance.
/// @param rel_tol  Relative tolerance.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_equal_float(const float expected, const float actual,
                          const float abs_tol, const float rel_tol,
                          const int verbosity) {
    try {
        fortest_assert()->assert_equal(
            expected, actual,
            abs_tol, rel_tol,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_float");
    }
}

///
/// @brief Assert that two C strings are equal.
/// @param expected Pointer to the expected null-terminated string.
/// @param actual   Pointer to the actual null-terminated string.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_equal_string(const char *expected, const char *actual, const int verbosity) {
    try {
        fortest_assert()->assert_equal(
            std::string(expected),
            std::string(actual),

            0.0, 0.0,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_string");
    }
}

///
/// @brief Assert that two integers are not equal.
/// @param expected First integer value.
/// @param actual   Second integer value.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_not_equal_int(const int expected, const int actual, const int verbosity) {
    try {
        fortest_assert()->assert_not_equal(
            expected, actual,
            0.0, 0.0,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_int");
    }
}

///
/// @brief Assert that two doubles are not equal within tolerances.
/// @param expected First double value.
/// @param actual   Second double value.
/// @param abs_tol  Absolute tolerance.
/// @param rel_tol  Relative tolerance.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_not_equal_double(const double expected, const double actual,
                               const double abs_tol, const double rel_tol,
                               const int verbosity) {
    try {
        fortest_assert()->assert_not_equal(
            expected, actual,
            abs_tol, rel_tol,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_double");
    }
}

///
/// @brief Assert that two floats are not equal within tolerances.
/// @param expected First float value.
/// @param actual   Second float value.
/// @param abs_tol  Absolute tolerance.
/// @param rel_tol  Relative tolerance.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_not_equal_float(const float expected, const float actual,
                              const float abs_tol, const float rel_tol,
                              const int verbosity) {
    try {
        fortest_assert()->assert_not_equal(
            expected, actual,
            abs_tol, rel_tol,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_float");
    }
}

///
/// @brief Assert that two C strings are not equal.
/// @param expected Pointer to the first null-terminated string.
/// @param actual   Pointer to the second null-terminated string.
/// @param verbosity Verbosity level (0=QUIET, 1=FAIL_ONLY, 2=ALL).
///
void c_assert_not_equal_string(const char *expected, const char *actual, const int verbosity) {
    try {
        fortest_assert()->assert_not_equal(
            std::string(expected),
            std::string(actual),

            0.0, 0.0,
            static_cast<Fortest::Verbosity>(verbosity)
        );
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_string");
    }
}
} // extern "C"

#endif // C_ASSERT_HPP
