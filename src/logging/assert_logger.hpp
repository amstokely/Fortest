#ifndef FORTEST_ASSERT_LOGGER_HPP
#define FORTEST_ASSERT_LOGGER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>

namespace Fortest {
    /// @brief Logger specifically tailored for assertions.
    ///
    /// Provides clear, consistent reporting of assertion results,
    /// with optional ANSI colors and internal storage of results.
    class AssertLogger {
    public:
        enum class Color {
            DEFAULT,
            RED,
            GREEN,
            YELLOW
        };

        struct Entry {
            std::string tag; ///< PASS/FAIL/etc.
            std::string msg; ///< Assertion message.
        };


        /// @brief Construct an AssertLogger.
        ///
        /// @param out Stream to write logs (default: std::cout).
        /// @param use_color Whether to print ANSI colors (default: true).
        explicit AssertLogger(std::ostream &out = std::cout, bool use_color = true)
            : m_out(out), m_use_color(use_color) {
        }

        /// @brief Log an assertion result.
        ///
        /// This function is designed to be called by the Assert class.
        /// @param msg The assertion message.
        /// @param tag Must be "PASS" or "FAIL" (others treated as INFO).
        /// @param border Optional border string (not used currently).
        void log(
            const std::string &msg,
            const std::string &tag,
            const std::optional<std::string> &border = std::nullopt
        ) {
            m_entries.push_back({tag, msg});
            if (border.has_value()) {
            }
            if (tag == "PASS") {
                write("PASS", msg, Color::GREEN);
            } else if (tag == "FAIL") {
                write("FAIL", msg, Color::RED);
            } else {
                write(tag, msg, Color::YELLOW);
            }
        }

        /// @brief Retrieve all log entries so far.
        [[nodiscard]] const std::vector<Entry> &entries() const { return m_entries; }

        /// @brief Print a summary of results.
        void print_summary() const {
            int passes = 0, fails = 0;
            for (auto &e: m_entries) {
                if (e.tag == "PASS") ++passes;
                if (e.tag == "FAIL") ++fails;
            }
            m_out << "Assertions Summary: "
                    << passes << " passed, "
                    << fails << " failed\n";
        }

    private:
        std::ostream &m_out;
        bool m_use_color;
        std::vector<Entry> m_entries;

        static std::string color_code(Color c) {
            switch (c) {
                case Color::RED: return "\033[31m";
                case Color::GREEN: return "\033[32m";
                case Color::YELLOW: return "\033[33m";
                default: return "\033[0m";
            }
        }

        void write(const std::string &tag, const std::string &msg, Color c) {
            if (m_use_color) {
                m_out << color_code(c) << "[ASSERT][" << tag << "] "
                        << msg << "\033[0m\n";
            } else {
                m_out << "[ASSERT][" << tag << "] " << msg << "\n";
            }
        }
    };
} // namespace Fortest

#endif //FORTEST_ASSERT_LOGGER_HPP
