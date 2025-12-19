# FindSqlite.cmake
#
# Finds SQLite3 library and headers.
#
# Defines:
#   SQLite_FOUND
#   SQLite_INCLUDE_DIRS
#   SQLite_LIBRARIES
#   SQLite::SQLite3 (imported target)

find_path(
        SQLite_INCLUDE_DIR
        NAMES sqlite3.h
        PATH_SUFFIXES include
)

find_library(
        SQLite_LIBRARY
        NAMES sqlite3 libsqlite3
        PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
        SQLite
        REQUIRED_VARS SQLite_LIBRARY SQLite_INCLUDE_DIR
)

if(SQLite_FOUND)
    set(SQLite_INCLUDE_DIRS ${SQLite_INCLUDE_DIR})
    set(SQLite_LIBRARIES ${SQLite_LIBRARY})

    if(NOT TARGET SQLite::SQLite3)
        add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
        set_target_properties(SQLite::SQLite3 PROPERTIES
                IMPORTED_LOCATION "${SQLite_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${SQLite_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(SQLite_INCLUDE_DIR SQLite_LIBRARY)
