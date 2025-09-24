#ifndef FORTEST_FORTRAN_FIXTURE_EXTRACTOR_HPP
#define FORTEST_FORTRAN_FIXTURE_EXTRACTOR_HPP
#pragma once

#include <string>
#include <string_view>
#include <regex>
#include <vector>

namespace Fortest {
    /**
     * @brief Fixture information extracted from Fortran code.
     */
    struct FixtureInfo {
        std::string type_name; ///< e.g., suite_fixture_t
        std::string scope; ///< e.g., "suite"
        std::string setup; ///< e.g., setup_suite_fixture
        std::string teardown; ///< e.g., teardown_suite_fixture
    };

    /**
     * @brief Extracts fixture type/scope mappings from Fortran driver code.
     *
     * Scans for calls of the form:
     *
     *   call session%register_fixture( &
     *       setup    = setup_suite_fixture, &
     *       teardown = teardown_suite_fixture, &
     *       args     = suite_ptr, &
     *       scope    = "suite", &
     *       test_suite_name = "math_ops")
     *
     * @returns a vector of FixtureInfo with the type, scope, setup, and teardown.
     */
    class FortranFixtureExtractor {
    public:
        explicit FortranFixtureExtractor(std::string_view code)
            : m_code(code) {
        }

        /// @brief Parse the code and extract all fixture information.
        std::vector<FixtureInfo> extract() {
            std::vector<FixtureInfo> fixtures;

            std::sregex_iterator it(m_code.begin(), m_code.end(), m_fixture_re), end;
            for (; it != end; ++it) {
                FixtureInfo info;
                info.setup = (*it)[1];
                info.teardown = (*it)[2];
                info.scope = to_lower((*it)[3]);
                info.type_name = infer_type(info.setup);

                fixtures.push_back(std::move(info));
            }

            return fixtures;
        }

    private:
        std::string m_code;

        /// @brief Regex to match register_fixture calls with setup, teardown, and scope.
        std::regex m_fixture_re{
            R"regex(register_fixture\s*\([^)]*setup\s*=\s*(\w+)[^)]*teardown\s*=\s*(\w+)[^)]*scope\s*=\s*"(\w+)")regex",
            std::regex::icase
        };


        /// @brief Infer fixture type name from the setup routine name.
        static std::string infer_type(const std::string &setup_name) {
            // e.g., setup_suite_fixture -> suite_fixture_t
            std::string type = setup_name;
            if (type.rfind("setup_", 0) == 0) {
                type.erase(0, 6);
            }
            if (type.rfind("teardown_", 0) == 0) {
                type.erase(0, 9);
            }
            return type + "_t";
        }

        /// @brief Convert a string to lowercase.
        static std::string to_lower(const std::string &s) {
            std::string result = s;
            for (auto &c: result) {
                c = static_cast<char>(tolower(c));
            }
            return result;
        }
    };
}

#endif //FORTEST_FORTRAN_FIXTURE_EXTRACTOR_HPP
