#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <concepts>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <optional>

namespace Fortest {
    /**
     * @brief Concept to define a Logger-like interface.
     *
     * A type satisfies the LoggerLike concept if it provides a `log` method
     * that accepts a message and a tag, and returns void.
     *
     * @tparam T The type to be checked against the LoggerLike concept.
     */
    template<typename T>
    concept LoggerLike = requires(
        T logger, const std::string &msg, const std::string &tag,
        const std::optional<std::string> &border
    )
            {
                { logger.log(msg, tag, border) } -> std::same_as<void>;
            };

    /**
     * @brief A class for logging messages with optional formatting and colors.
     *
     * The Logger class provides functionality to log messages with different
     * tags (e.g., PASS, FAIL, INFO) and optional borders. It supports color
     * formatting for terminal output.
     */
    class Logger {
    public:
        /**
         * @brief Enum representing available text colors for logging.
         */
        enum class Color {
            DEFAULT, /**< Default terminal color. */
            RED, /**< Red color. */
            GREEN, /**< Green color. */
            YELLOW, /**< Yellow color. */
            BLUE, /**< Blue color. */
            MAGENTA, /**< Magenta color. */
            CYAN, /**< Cyan color. */
            WHITE /**< White color. */
        };

        /**
         * @brief Constructs a Logger instance.
         *
         * @param m_out The output stream to log messages to (default: std::cout).
         * @param m_border An optional border string to surround log messages.
         * @param m_color The default color for log messages (default: Color::DEFAULT).
         */
        explicit Logger(
            std::ostream &m_out = std::cout,
            std::string m_border = "",
            Color m_color = Color::DEFAULT
        )
            : m_out(m_out), m_border(std::move(m_border)), m_color(m_color) {
        }

        /**
         * @brief Logs a message with a specific tag and optional border.
         *
         * Depending on the tag, the message is formatted with a specific color.
         * Supported tags include PASS, FAIL, INFO, TRUE, and FALSE.
         *
         * @param msg The message to log.
         * @param tag The tag associated with the message.
         * @param border An optional border string to override the default border.
         */
        void log(
            const std::string &msg,
            const std::string &tag,
            const std::optional<std::string> &border = std::nullopt
        ) {
            m_last_msg = msg;
            m_last_tag = tag;

            if (tag == "PASS") {
                log_with_format("PASS", msg, Color::GREEN, border);
            } else if (tag == "FAIL") {
                log_with_format("FAIL", msg, Color::RED, border);
            } else if (tag == "INFO") {
                log_with_format("INFO", msg, Color::DEFAULT, border);
            } else if (tag == "TRUE") {
                log_with_format("TRUE", msg, Color::GREEN, border);
            } else if (tag == "FALSE") {
                log_with_format("FALSE", msg, Color::RED, border);
            } else {
                m_out << msg << '\n';
            }
        }

        /**
         * @brief Converts a Color enum value to its corresponding ANSI escape code.
         *
         * @param c The Color enum value.
         * @return The ANSI escape code as a string.
         */
        static std::string color_to_code(Color c) {
            switch (c) {
                case Color::RED: return "\033[31m";
                case Color::GREEN: return "\033[32m";
                case Color::YELLOW: return "\033[33m";
                case Color::BLUE: return "\033[34m";
                case Color::MAGENTA: return "\033[35m";
                case Color::CYAN: return "\033[36m";
                case Color::WHITE: return "\033[37m";
                default: return "\033[0m";
            }
        }

        /**
         * @brief Outputs the last logged message and tag to a stream.
         *
         * If no message has been logged, outputs "(no log yet)".
         *
         * @param os The output stream.
         * @param logger The Logger instance.
         * @return The output stream with the logged message.
         */
        friend std::ostream &operator<<(std::ostream &os, const Logger &logger) {
            if (!logger.m_last_tag.empty()) {
                os << "[" << logger.m_last_tag << "] " << logger.m_last_msg;
            } else {
                os << "(no log yet)";
            }
            return os;
        }

    private:
        std::ostream &m_out; /**< The output stream for logging. */
        std::string m_border; /**< The default border string for log messages. */
        Color m_color; /**< The current color for log messages. */

        std::string m_last_msg; /**< The last logged message. */
        std::string m_last_tag; /**< The tag associated with the last logged message. */

        /**
         * @brief Logs a message with a specific format and color.
         *
         * @param label The label to display (e.g., PASS, FAIL).
         * @param msg The message to log.
         * @param color The color to use for the message.
         * @param border_override An optional border string to override the default border.
         */
        void log_with_format(
            const std::string &label,
            const std::string &msg,
            Color color,
            const std::optional<std::string> &border_override
        ) {
            m_color = color;
            std::string color_code = color_to_code(m_color);
            std::string reset_code = "\033[0m";

            // pick effective border
            const std::string &effective_border =
                    border_override.has_value() ? *border_override : m_border;

            if (!effective_border.empty()) {
                m_out << color_code << effective_border << reset_code << '\n';
            }
            m_out << color_code << "[" << label << "] " << msg << reset_code << '\n';
            if (!effective_border.empty() && !m_border.empty() ) {
                m_out << color_code << reset_code << '\n';
            }
        }
    };
}

#endif // LOGGING_HPP
