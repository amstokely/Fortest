add_executable(test_math_ops_fortran math_ops.test.f90 math_ops.fixture.f90 math_ops_mod.f90)
set_target_properties(test_math_ops_fortran PROPERTIES
        LINKER_LANGUAGE Fortran
)
target_link_libraries(test_math_ops_fortran PRIVATE fortest)
add_test(NAME test_math_ops_fortran COMMAND test_math_ops_fortran)