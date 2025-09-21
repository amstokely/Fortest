#ifndef FORTEST_C_TEST_SESSION_H
#define FORTEST_C_TEST_SESSION_H

#include <string>
#include <iostream>
#include <cstdlib>
#include "g_assert.hpp"
#include "g_logging.hpp"
#include "g_test_session.hpp"
#include "fixture.hpp"

extern "C" {

/**
 * @brief Helper: Print exception message and terminate.
 */
inline void fortest_fatal_terminate(const char *func) noexcept {
    try {
        throw; // rethrow current exception
    } catch (const std::exception &e) {
        std::cerr << "[FORTEST FATAL] Exception in " << func
                  << ": " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FORTEST FATAL] Unknown exception in " << func
                  << std::endl;
    }
    std::terminate();
}

/**
 * @brief Register a new test suite with the global session.
 */
void c_register_test_suite(const char *name) {
    try {
        Fortest::GlobalTestSession::instance().add_test_suite(name);
    } catch (...) {
        fortest_fatal_terminate("c_register_test_suite");
    }
}

/**
 * @brief Register a fixture for a given test suite.
 */
void c_register_fixture(
    const char *suite_name, void *setup_ptr, void *teardown_ptr,
    void *args_ptr, const char *scope
) {
    try {
        auto setup = reinterpret_cast<void(*)(void *)>(setup_ptr);
        auto teardown = reinterpret_cast<void(*)(void *)>(teardown_ptr);

        Fortest::Scope scope_enum = Fortest::Scope::Test;
        if (std::string(scope) == "test") {
            scope_enum = Fortest::Scope::Test;
        } else if (std::string(scope) == "suite") {
            scope_enum = Fortest::Scope::Suite;
        } else if (std::string(scope) == "session") {
            scope_enum = Fortest::Scope::Session;
        }

        if (std::string(suite_name).empty() &&
            scope_enum == Fortest::Scope::Session) {
            Fortest::GlobalTestSession::instance().add_fixture(
                Fortest::Fixture<void>(setup, teardown, args_ptr, scope_enum)
            );
        } else {
            Fortest::GlobalTestSession::instance().add_fixture(
                suite_name,
                Fortest::Fixture<void>(setup, teardown, args_ptr, scope_enum)
            );
        }
    } catch (...) {
        fortest_fatal_terminate("c_register_fixture");
    }
}

/**
 * @brief Register a test case with the given suite.
 */
void c_register_test(
    const char *suite_name, const char *test_name, void *test_ptr
) {
    try {
        auto test = reinterpret_cast<void(*)(void *, void *, void *)>(test_ptr);
        Fortest::GlobalTestSession::instance().add_test(
            suite_name, test_name, test
        );
    } catch (...) {
        fortest_fatal_terminate("c_register_test");
    }
}

/**
 * @brief Run all registered tests in the global session.
 */
void c_run_test_session() {
    try {
        const auto logger = Fortest::GlobalLogger::instance();
        Fortest::GlobalTestSession::instance().run(logger);
    } catch (...) {
        fortest_fatal_terminate("c_run_test_session");
    }
}

/**
 * @brief Get the overall status of a test suite.
 */
int c_get_test_suite_status(const char *name) {
    try {
        int status = 0;
        auto statuses =
            Fortest::GlobalTestSession::instance().get_test_suite_status(name);

        for (const auto &test_status : statuses | std::views::values) {
            if (test_status == Fortest::Test::Status::FAIL) {
                status = 1;
                break;
            }
        }
        return status;
    } catch (...) {
        fortest_fatal_terminate("c_get_test_suite_status");
    }
}
} // extern "C"

#endif // FORTEST_C_TEST_SESSION_H
