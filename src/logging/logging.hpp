#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <concepts>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>

namespace Fortest {
    /// @brief Concept that defines the minimal Logger interface.
    ///
    /// A type satisfies LoggerLike if it provides:
    /// ```
    /// void log(const std::string &msg, const std::string &tag);
    /// ```
    template<typename T>
    concept LoggerLike = requires(
        T logger, const std::string &msg, const std::string &tag
    )
    {
        { logger.log(msg, tag) } -> std::same_as<void>;
    };

    /// @brief Default logger implementation that writes formatted,
    /// color-coded output to an ostream.
    ///
    /// The logger stores the last log message and tag, supports
    /// ANSI color output, and can optionally draw borders around
    /// log messages. Used by the framework to provide consistent
    /// reporting of test results and status messages.
    class Logger {
    public:
        /// @brief Supported colors for console output.
        enum class Color {
            DEFAULT, ///< No color / reset to default.
            RED,     ///< Red text (failures, false).
            GREEN,   ///< Green text (success, true).
            YELLOW,  ///< Yellow text.
            BLUE,    ///< Blue text.
            MAGENTA, ///< Magenta text.
            CYAN,    ///< Cyan text.
            WHITE    ///< White text.
        };

        /// @brief Construct a Logger.
        ///
        /// @param out Stream to write log output (default: std::cout).
        /// @param border Optional border string printed above/below messages.
        /// @param color Initial text color (default: Color::DEFAULT).
        explicit Logger(
            std::ostream &out = std::cout,
            std::string border = "",
            Color color = Color::DEFAULT
        )
            : out_(out), border_(std::move(border)), color_(color) {
        }

        /// @brief Log a message with a tag.
        ///
        /// Special handling for recognized tags:
        /// - "PASS" → green
        /// - "FAIL" → red
        /// - "INFO" → default color
        /// - "TRUE" → green
        /// - "FALSE" → red
        ///
        /// Other tags print the message as plain text.
        /// @param msg The message string.
        /// @param tag The message tag.
        void log(const std::string &msg, const std::string &tag) {
            last_msg_ = msg;
            last_tag_ = tag;

            if (tag == "PASS") {
                log_with_format("PASS", msg, Color::GREEN);
            } else if (tag == "FAIL") {
                log_with_format("FAIL", msg, Color::RED);
            } else if (tag == "INFO") {
                log_with_format("INFO", msg, Color::DEFAULT);
            } else if (tag == "TRUE") {
                log_with_format("TRUE", msg, Color::GREEN);
            } else if (tag == "FALSE") {
                log_with_format("FALSE", msg, Color::RED);
            } else {
                out_ << msg << '\n';
            }
        }

        /// @brief Convert a color enum to an ANSI escape code.
        /// @param c The color.
        /// @return Corresponding ANSI escape code string.
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

        /// @brief Stream insertion operator.
        ///
        /// Prints the last logged message and tag in the form:
        /// ```
        /// [TAG] message
        /// ```
        /// If no log has been recorded yet, prints `(no log yet)`.
        friend std::ostream &operator<<(std::ostream &os, const Logger &logger) {
            if (!logger.last_tag_.empty()) {
                os << "[" << logger.last_tag_ << "] " << logger.last_msg_;
            } else {
                os << "(no log yet)";
            }
            return os;
        }

    private:
        std::ostream &out_;   ///< Output stream reference.
        std::string border_;  ///< Optional border string.
        Color color_;         ///< Current color for logging.

        std::string last_msg_; ///< Last logged message.
        std::string last_tag_; ///< Last logged tag.

        /// @brief Internal helper for formatted log output.
        ///
        /// Wraps the message with color codes and optional borders.
        /// @param label Tag label (PASS/FAIL/etc.).
        /// @param msg Log message.
        /// @param color Text color to apply.
        void log_with_format(
            const std::string &label,
            const std::string &msg,
            Color color
        ) {
            color_ = color;
            std::string color_code = color_to_code(color_);
            std::string reset_code = "\033[0m";

            if (!border_.empty()) {
                out_ << color_code << border_ << reset_code << '\n';
            }
            out_ << color_code << "[" << label << "] " << msg << reset_code << '\n';
            if (!border_.empty()) {
                out_ << color_code << border_ << reset_code << '\n';
            }
        }
    };
}

#endif // LOGGING_HPP
