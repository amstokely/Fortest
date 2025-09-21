#ifndef F_C_STRING_TEST_HPP
#define F_C_STRING_TEST_HPP

extern "C" {

    /// @brief Check if the given C string equals "abc".
    /// @param c_string A null-terminated C string pointer.
    /// @return 0 if equal to "abc", 1 otherwise.
    int str_equals_abc(const char* c_string);

    /// @brief Check if the given C string equals "abc def".
    /// @param c_string A null-terminated C string pointer.
    /// @return 0 if equal to "abc def", 1 otherwise.
    int str_equals_abc_space_def(const char* c_string);
} // extern "C"

#endif //F_C_STRING_TEST_HPP
