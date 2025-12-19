# --------------------
# C++ side
# --------------------
add_library(cpp_fortest SHARED
        test_suite/test_suite.cpp
        assert/assert.cpp
        logging/logging.cpp
        logging/assert_logger.hpp
        test/test.hpp
        test/parameterized_test.hpp
        test_session/test_session.hpp
        fixture/fixture.hpp
        db/db.cpp
)
target_link_libraries(cpp_fortest PUBLIC SQLite::SQLite3)

target_include_directories(cpp_fortest
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/assert>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/logging>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test_suite>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test_session>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utils>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/db>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fixture>
        $<INSTALL_INTERFACE:include/fortest>)

add_library(c_fortest SHARED
        assert/c_assert.cpp
        test_session/c_test_session.h
        test_session/c_test_session.cpp
)
target_include_directories(c_fortest
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/assert>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/logging>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test_suite>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test_session>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utils>
        $<INSTALL_INTERFACE:include/fortest>)
target_link_libraries(c_fortest PUBLIC cpp_fortest)

# --------------------
# Fortran side
# --------------------
add_library(fortest SHARED
        test_suite/test_suite_mod.f90
        test_session/test_session_mod.f90
        assert/assert_mod.f90
        utils/f_c_string_t_mod.f90
        utils/procedure_interfaces_mod.f90
)
set_target_properties(fortest PROPERTIES
        LINKER_LANGUAGE Fortran
        Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/mod
)
target_link_libraries(fortest
        PUBLIC c_fortest           # and on cpp_fortest directly
)
target_include_directories(fortest
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/mod>
        $<INSTALL_INTERFACE:include/fortest/mod>
)

# --------------------
# Install libraries and export targets
# --------------------
install(TARGETS fortest cpp_fortest c_fortest
        EXPORT FortestTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

# --------------------
# Install headers
# --------------------
install(FILES
        assert/assert.hpp
        assert/c_assert.h
        assert/g_assert.hpp
        logging/logging.hpp
        logging/g_logging.hpp
        logging/assert_logger.hpp
        test_suite/test_suite.hpp
        test/test.hpp
        test/parameterized_test.hpp
        test_session/test_session.hpp
        test_session/c_test_session.h
        utils/global_base.hpp
        fixture/fixture.hpp
        db/db.hpp
        DESTINATION include/fortest
)

# --------------------
# Install Fortran modules (.mod files)
# --------------------
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/mod/
        DESTINATION include/fortest/mod
        FILES_MATCHING PATTERN "*.mod"
)

# --------------------
# Export CMake package
# --------------------
install(EXPORT FortestTargets
        FILE FortestTargets.cmake
        NAMESPACE Fortest::
        DESTINATION lib/cmake/Fortest
)
