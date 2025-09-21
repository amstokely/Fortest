#include "f_c_string.test.h"
#include <string>

/// @brief Check if the C string is equal to "abc".
/// @param c_string Null-terminated C string from Fortran.
/// @return 0 if match, 1 otherwise.
int str_equals_abc(const char* c_string) {
    std::string str(c_string);
    if (str == "abc") {
        return 0;
    }
    return 1;
}

/// @brief Check if the C string is equal to "abc def".
/// @param c_string Null-terminated C string from Fortran.
/// @return 0 if match, 1 otherwise.
int str_equals_abc_space_def(const char* c_string) {
    std::string str(c_string);
    if (str == "abc def") {
        return 0;
    }
    return 1;
}
