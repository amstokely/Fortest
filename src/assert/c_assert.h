#ifndef C_ASSERT_HPP
#define C_ASSERT_HPP

#include "assert.hpp"
#include "g_logging.hpp"
#include "g_assert.hpp"

#include <iostream>
#include <cstdlib>

/// @file c_assert.hpp
/// @brief C bindings for the Fortest assertion framework.
///
/// These functions provide a C-compatible interface to the Fortest
/// assertion utilities. Each function delegates to the global
/// Fortest logger and assertion engine. All exceptions are caught,
/// printed to `stderr`, and cause the program to terminate.
/// This ensures no C++ exception ever crosses the C boundary.

extern "C" {

/**
 * @brief Helper: Get the global Fortest logger instance.
 *
 * @return Shared pointer to the global logger.
 */
inline auto fortest_logger() {
    return Fortest::GlobalLogger::instance();
}

/**
 * @brief Helper: Get the global Fortest assert instance.
 *
 * @return Shared pointer to the global assertion engine.
 */
inline auto fortest_assert() {
    return Fortest::GlobalAssert::instance();
}

/**
 * @brief Helper: Print the current exception and terminate the program.
 *
 * This function is used internally by all C bindings to ensure that
 * any C++ exception is caught and converted into a fatal error.
 *
 * @param func Name of the C wrapper function where the exception occurred.
 */
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

/**
 * @brief Assert that a condition is true.
 *
 * @param condition Integer treated as boolean (non-zero = true).
 */
void c_assert_true(const int condition) {
    try {
        fortest_assert()->assert_true(condition, fortest_logger());
    } catch (...) {
        fortest_c_assert_fatal("c_assert_true");
    }
}

/**
 * @brief Assert that a condition is false.
 *
 * @param condition Integer treated as boolean (zero = false).
 */
void c_assert_false(const int condition) {
    try {
        fortest_assert()->assert_false(condition, fortest_logger());
    } catch (...) {
        fortest_c_assert_fatal("c_assert_false");
    }
}

/**
 * @brief Assert that two integers are equal.
 *
 * @param expected Expected integer value.
 * @param actual   Actual integer value.
 */
void c_assert_equal_int(const int expected, const int actual) {
    try {
        fortest_assert()->assert_equal(expected, actual, fortest_logger());
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_int");
    }
}

/**
 * @brief Assert that two doubles are equal within tolerances.
 *
 * @param expected Expected double value.
 * @param actual   Actual double value.
 * @param abs_tol  Absolute tolerance.
 * @param rel_tol  Relative tolerance.
 */
void c_assert_equal_double(const double expected, const double actual,
                           const double abs_tol, const double rel_tol) {
    try {
        fortest_assert()->assert_equal(expected, actual, fortest_logger(),
                                       abs_tol, rel_tol);
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_double");
    }
}

/**
 * @brief Assert that two floats are equal within tolerances.
 *
 * @param expected Expected float value.
 * @param actual   Actual float value.
 * @param abs_tol  Absolute tolerance.
 * @param rel_tol  Relative tolerance.
 */
void c_assert_equal_float(const float expected, const float actual,
                          const float abs_tol, const float rel_tol) {
    try {
        fortest_assert()->assert_equal(expected, actual, fortest_logger(),
                                       abs_tol, rel_tol);
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_float");
    }
}

/**
 * @brief Assert that two C strings are equal.
 *
 * @param expected Pointer to the expected null-terminated string.
 * @param actual   Pointer to the actual null-terminated string.
 */
void c_assert_equal_string(const char *expected, const char *actual) {
    try {
        fortest_assert()->assert_equal(std::string(expected),
                                       std::string(actual),
                                       fortest_logger());
    } catch (...) {
        fortest_c_assert_fatal("c_assert_equal_string");
    }
}

/**
 * @brief Assert that two integers are not equal.
 *
 * @param expected First integer value.
 * @param actual   Second integer value.
 */
void c_assert_not_equal_int(const int expected, const int actual) {
    try {
        fortest_assert()->assert_not_equal(expected, actual, fortest_logger());
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_int");
    }
}

/**
 * @brief Assert that two doubles are not equal within tolerances.
 *
 * @param expected First double value.
 * @param actual   Second double value.
 * @param abs_tol  Absolute tolerance.
 * @param rel_tol  Relative tolerance.
 */
void c_assert_not_equal_double(const double expected, const double actual,
                               const double abs_tol, const double rel_tol) {
    try {
        fortest_assert()->assert_not_equal(expected, actual, fortest_logger(),
                                           abs_tol, rel_tol);
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_double");
    }
}

/**
 * @brief Assert that two floats are not equal within tolerances.
 *
 * @param expected First float value.
 * @param actual   Second float value.
 * @param abs_tol  Absolute tolerance.
 * @param rel_tol  Relative tolerance.
 */
void c_assert_not_equal_float(const float expected, const float actual,
                              const float abs_tol, const float rel_tol) {
    try {
        fortest_assert()->assert_not_equal(expected, actual, fortest_logger(),
                                           abs_tol, rel_tol);
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_float");
    }
}

/**
 * @brief Assert that two C strings are not equal.
 *
 * @param expected Pointer to the first null-terminated string.
 * @param actual   Pointer to the second null-terminated string.
 */
void c_assert_not_equal_string(const char *expected, const char *actual) {
    try {
        fortest_assert()->assert_not_equal(std::string(expected),
                                           std::string(actual),
                                           fortest_logger());
    } catch (...) {
        fortest_c_assert_fatal("c_assert_not_equal_string");
    }
}

} // extern "C"

#endif // C_ASSERT_HPP
