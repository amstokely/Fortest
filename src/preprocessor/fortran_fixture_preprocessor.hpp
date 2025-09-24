#ifndef FORTEST_FORTRAN_FIXTURE_PREPROCESSOR_HPP
#define FORTEST_FORTRAN_FIXTURE_PREPROCESSOR_HPP

#include <string>
#include <string_view>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include "preprocessor_base.hpp"

namespace Fortest {
/**
 * @brief Fortran fixture preprocessor.
 *
 * Transforms fixture type definitions into setup/teardown
 * routines that handle c_ptr plumbing. Uses the same CRTP
 * base as the test preprocessor.
 */
class FortranFixturePreprocessor : public PreprocessorBase<FortranFixturePreprocessor> {
public:
    explicit FortranFixturePreprocessor(std::unordered_map<std::string, std::string> scope_map)
        : m_scope_map(std::move(scope_map)) {}

    /// @brief Transform Fortran fixture type definitions.
    std::string transform(const std::string & input) {
        std::string code(input);
        std::ostringstream output;

        std::sregex_iterator it(code.begin(), code.end(), m_type_re), end;
        size_t last_pos = 0;

        for (; it != end; ++it) {
            auto match = *it;
            output << code.substr(last_pos, match.position() - last_pos);

            std::string type_name = match[1];
            std::string body = match[2];

            output << generate_fixture_routines(type_name, body);
            last_pos = match.position() + match.length();
        }

        output << code.substr(last_pos);
        return output.str();
    }

private:
    std::unordered_map<std::string, std::string> m_scope_map;

    std::regex m_type_re{
        R"(type\s*::\s*(\w+)([\s\S]*?)end\s+type)",
        std::regex::icase
    };

    std::string generate_fixture_routines(const std::string &type_name,
                                          const std::string & /*body*/) {
        std::ostringstream out;
        std::string scope = resolve_scope(type_name);

        std::string lower = to_lower(scope);
        std::string setup_name = "setup_" + lower + "_fixture";
        std::string teardown_name = "teardown_" + lower + "_fixture";

        // Setup routine
        out << "   subroutine " << setup_name << "(args)\n";
        out << "      type(c_ptr), value :: args\n";
        out << "      type(" << type_name << "), pointer :: fix\n";
        out << "      call c_f_pointer(args, fix)\n";
        out << "      ! allocate and initialize " << type_name << " here\n";
        out << "   end subroutine " << setup_name << "\n\n";

        // Teardown routine
        out << "   subroutine " << teardown_name << "(args)\n";
        out << "      type(c_ptr), value :: args\n";
        out << "      type(" << type_name << "), pointer :: fix\n";
        out << "      call c_f_pointer(args, fix)\n";
        out << "      ! deallocate " << type_name << " fields here\n";
        out << "   end subroutine " << teardown_name << "\n\n";

        return out.str();
    }

    std::string resolve_scope(const std::string &type_name) const {
        auto it = m_scope_map.find(type_name);
        if (it == m_scope_map.end()) {
            throw std::runtime_error("Unknown fixture type: " + type_name);
        }
        return it->second;
    }

    static std::string to_lower(const std::string &s) {
        std::string result = s;
        for (auto &c : result) c = static_cast<char>(tolower(c));
        return result;
    }
};
}


#endif //FORTEST_FORTRAN_FIXTURE_PREPROCESSOR_HPP