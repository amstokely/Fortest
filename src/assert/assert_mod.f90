!> @brief Assertion utilities for Fortran tests with verbosity.
!>
!> @details
!> This module provides typed assertion routines that wrap C
!> implementations for consistency and integration with the Fortest
!> framework. Supported assertions include:
!> - Equality and inequality for integers, reals, doubles, and strings
!> - Boolean checks (`assert_true`, `assert_false`)
!>
!> Verbosity control:
!> - 0 = QUIET     (no output, even on failure)
!> - 1 = FAIL_ONLY (default, print only failures)
!> - 2 = ALL       (print on pass and fail)
!>
!> Each routine accepts an optional `verbosity` argument to override
!> the global default.
module fortest_assert
    use iso_c_binding, only : c_int, c_float, c_double, c_ptr
    use f_c_string_t_mod, only : f_c_string_t

    implicit none
    private

    public :: assert_equal
    public :: assert_not_equal
    public :: assert_true
    public :: assert_false

    integer, parameter, public :: VERBOSITY_QUIET = 0
    integer, parameter, public :: VERBOSITY_FAIL_ONLY = 1
    integer, parameter, public :: VERBOSITY_ALL = 2

    !> @brief Assert equality of two values.
    interface assert_equal
        module procedure assert_equal_int
        module procedure assert_equal_logical
        module procedure assert_equal_float
        module procedure assert_equal_double
        module procedure assert_equal_string
    end interface assert_equal

    !> @brief Assert inequality of two values.
    interface assert_not_equal
        module procedure assert_not_equal_int
        module procedure assert_not_equal_logical
        module procedure assert_not_equal_float
        module procedure assert_not_equal_double
        module procedure assert_not_equal_string
    end interface assert_not_equal

contains

    !> @brief Assert that two integers are equal.
    !> @param expected Expected integer value.
    !> @param actual   Actual integer value.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_equal_int(expected, actual, verbosity)
        integer(c_int), intent(in) :: expected, actual
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        interface
            subroutine c_assert_equal_int(expected, actual, verbosity) bind(C)
                import :: c_int
                integer(c_int), value :: expected, actual, verbosity
            end subroutine c_assert_equal_int
        end interface
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if
        call c_assert_equal_int(expected, actual, verbosity_level)
    end subroutine assert_equal_int

    !> @brief Assert that two integers are equal.
    !> @param expected Expected integer value.
    !> @param actual   Actual integer value.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_equal_logical(expected, actual, verbosity)
        logical, intent(in) :: expected, actual
        integer(c_int) :: i_expected, i_actual
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level

        i_expected = merge(1_c_int, 0_c_int, expected)
        i_actual = merge(1_c_int, 0_c_int, actual)
        call assert_equal(i_expected, i_actual, verbosity)
    end subroutine assert_equal_logical

    !> @brief Assert that two real (single precision) values are equal.
    !> Supports absolute and relative tolerance.
    !> @param expected Expected real value.
    !> @param actual   Actual real value.
    !> @param abs_tol  Absolute tolerance (optional).
    !> @param rel_tol  Relative tolerance (optional).
    !> @param verbosity Verbosity level (optional).
    subroutine assert_equal_float(expected, actual, abs_tol, rel_tol, verbosity)
        use iso_c_binding, only : c_float, c_double
        real(c_float), intent(in) :: expected, actual
        real(c_float), intent(in), optional :: abs_tol, rel_tol
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        real(c_float) :: a_tol, r_tol

        interface
            subroutine c_assert_equal_float(expected, actual, abs_tol, rel_tol, verbosity) bind(C)
                import :: c_float, c_double, c_int
                real(c_float), value :: expected, actual
                real(c_float), value :: abs_tol, rel_tol
                integer(c_int), value :: verbosity
            end subroutine c_assert_equal_float
        end interface

        ! Default tolerances to zero if not provided
        a_tol = 0.0
        r_tol = 0.0
        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY

        if (present(abs_tol)) a_tol = abs_tol
        if (present(rel_tol)) r_tol = rel_tol
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if

        call c_assert_equal_float(expected, actual, a_tol, r_tol, verbosity_level)
    end subroutine assert_equal_float


    !> @brief Assert that two double precision values are equal.
    !> Supports absolute and relative tolerance.
    !> @param expected Expected double precision value.
    !> @param actual   Actual double precision value.
    !> @param abs_tol  Absolute tolerance (optional).
    !> @param rel_tol  Relative tolerance (optional).
    !> @param verbosity Verbosity level (optional).
    subroutine assert_equal_double(expected, actual, abs_tol, rel_tol, verbosity)
        use iso_c_binding, only : c_double
        real(c_double), intent(in) :: expected, actual
        real(c_double), intent(in), optional :: abs_tol, rel_tol
        real(c_double) :: a_tol, r_tol
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level

        interface
            subroutine c_assert_equal_double(expected, actual, abs_tol, rel_tol, verbosity) bind(C)
                import :: c_double, c_int
                real(c_double), value :: expected, actual
                real(c_double), value :: abs_tol, rel_tol
                integer(c_int), value :: verbosity
            end subroutine c_assert_equal_double
        end interface

        ! Default tolerances to zero if not provided
        a_tol = 0.0d0
        r_tol = 0.0d0
        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(abs_tol)) a_tol = abs_tol
        if (present(rel_tol)) r_tol = rel_tol
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if

        call c_assert_equal_double(expected, actual, a_tol, r_tol, verbosity_level)
    end subroutine assert_equal_double


    !> @brief Assert that two character strings are equal.
    !> @param expected Expected string value.
    !> @param actual   Actual string value.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_equal_string(expected, actual, verbosity)
        character(len = *), intent(in) :: expected, actual
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        type(f_c_string_t) :: f_c_string_expected, f_c_string_actual
        integer :: status
        interface
            subroutine c_assert_equal_string(expected, actual, verbosity) bind(C)
                import :: c_ptr, c_int
                type(c_ptr), value, intent(in) :: expected, actual
                integer(c_int), value :: verbosity
            end subroutine c_assert_equal_string
        end interface

        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if
        f_c_string_expected = f_c_string_t(expected)
        f_c_string_actual = f_c_string_t(actual)
        status = f_c_string_expected%to_c()
        status = f_c_string_actual%to_c()
        call c_assert_equal_string(&
                f_c_string_expected%get_c_string(), &
                f_c_string_actual%get_c_string(), &
                verbosity_level)
    end subroutine assert_equal_string

    !> @brief Assert that two integers are not equal.
    !> @param expected Integer value to compare.
    !> @param actual   Integer value to compare.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_not_equal_int(expected, actual, verbosity)
        integer(c_int), intent(in) :: expected, actual
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        interface
            subroutine c_assert_not_equal_int(expected, actual, verbosity) bind(C)
                import :: c_int
                integer(c_int), value :: expected, actual
                integer(c_int), value :: verbosity
            end subroutine c_assert_not_equal_int
        end interface

        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if
        call c_assert_not_equal_int(expected, actual, verbosity_level)
    end subroutine assert_not_equal_int

    subroutine assert_not_equal_logical(expected, actual, verbosity)
        logical, intent(in) :: expected, actual
        integer(c_int) :: i_expected, i_actual
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        i_expected = merge(1_c_int, 0_c_int, expected)
        i_actual = merge(1_c_int, 0_c_int, actual)
        call assert_not_equal(i_expected, i_actual, verbosity)
    end subroutine assert_not_equal_logical

    !> @brief Assert that two real (single precision) values are not equal.
    !> Supports absolute and relative tolerance.
    !> @param expected Real value to compare.
    !> @param actual   Real value to compare.
    !> @param abs_tol  Absolute tolerance.
    !> @param rel_tol  Relative tolerance.
    subroutine assert_not_equal_float(expected, actual, abs_tol, rel_tol, verbosity)
        use iso_c_binding, only : c_float, c_double
        real(c_float), intent(in) :: expected, actual
        real(c_float), intent(in), optional :: abs_tol, rel_tol
        real(c_float) :: a_tol, r_tol
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level

        interface
            subroutine c_assert_not_equal_float(expected, actual, abs_tol, rel_tol, verbosity) bind(C)
                import :: c_float, c_double, c_int
                real(c_float), value :: expected, actual
                real(c_float), value :: abs_tol, rel_tol
                integer(c_int), value :: verbosity
            end subroutine c_assert_not_equal_float
        end interface

        ! Default tolerances to zero if not provided
        a_tol = 0.0_c_float
        r_tol = 0.0_c_float
        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(abs_tol)) a_tol = abs_tol
        if (present(rel_tol)) r_tol = rel_tol
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if

        call c_assert_not_equal_float(expected, actual, a_tol, r_tol, verbosity_level)
    end subroutine assert_not_equal_float


    !> @brief Assert that two double precision values are not equal.
    !> Supports absolute and relative tolerance.
    !> @param expected Double precision value to compare.
    !> @param actual   Double precision value to compare.
    !> @param abs_tol  Absolute tolerance.
    !> @param rel_tol  Relative tolerance.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_not_equal_double(expected, actual, abs_tol, rel_tol, verbosity)
        use iso_c_binding, only : c_double
        real(c_double), intent(in) :: expected, actual
        real(c_double), intent(in), optional :: abs_tol, rel_tol
        real(c_double) :: a_tol, r_tol
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level

        interface
            subroutine c_assert_not_equal_double(expected, actual, abs_tol, rel_tol, verbosity) bind(C)
                import :: c_double, c_int
                real(c_double), value :: expected, actual
                real(c_double), value :: abs_tol, rel_tol
                integer(c_int), value :: verbosity
            end subroutine c_assert_not_equal_double
        end interface

        ! Default tolerances to zero if not provided
        a_tol = 0.0d0
        r_tol = 0.0d0
        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(abs_tol)) a_tol = abs_tol
        if (present(rel_tol)) r_tol = rel_tol
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if

        call c_assert_not_equal_double(expected, actual, a_tol, r_tol, verbosity_level)
    end subroutine assert_not_equal_double


    !> @brief Assert that two character strings are not equal.
    !> @param expected String value to compare.
    !> @param actual   String value to compare.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_not_equal_string(expected, actual, verbosity)
        character(len = *), intent(in) :: expected, actual
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        type(f_c_string_t) :: f_c_string_expected, f_c_string_actual
        integer :: status
        interface
            subroutine c_assert_not_equal_string(expected, actual, verbosity) bind(C)
                import :: c_ptr, c_int
                type(c_ptr), value, intent(in) :: expected, actual
                integer(c_int), value :: verbosity
            end subroutine c_assert_not_equal_string
        end interface

        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if
        f_c_string_expected = f_c_string_t(expected)
        f_c_string_actual = f_c_string_t(actual)
        status = f_c_string_expected%to_c()
        status = f_c_string_actual%to_c()
        call c_assert_not_equal_string(&
                f_c_string_expected%get_c_string(), &
                f_c_string_actual%get_c_string(), &
                verbosity_level)
    end subroutine assert_not_equal_string

    !> @brief Assert that a logical condition is true.
    !> @param condition Logical condition to check.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_true(condition, verbosity)
        logical, intent(in) :: condition
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        integer(c_int) :: i_condition
        interface
            subroutine c_assert_true(condition, verbosity) bind(C, name = "c_assert_true")
                import :: c_int
                integer(c_int), value :: condition
                integer(c_int), value :: verbosity
            end subroutine c_assert_true
        end interface
        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if

        i_condition = merge(1_c_int, 0_c_int, condition)
        call c_assert_true(i_condition, verbosity_level)
    end subroutine assert_true

    !> @brief Assert that a logical condition is false.
    !> @param condition Logical condition to check.
    !> @param verbosity Verbosity level (optional).
    subroutine assert_false(condition, verbosity)
        logical, intent(in) :: condition
        integer(c_int), intent(in), optional :: verbosity
        integer(c_int) :: verbosity_level
        integer(c_int) :: i_condition
        interface
            subroutine c_assert_false(condition, verbosity) bind(C, name = "c_assert_false")
                import :: c_int
                integer(c_int), value :: condition
                integer(c_int), value :: verbosity
            end subroutine c_assert_false
        end interface
        ! Default verbosity to FAIL_ONLY if not provided
        verbosity_level = VERBOSITY_FAIL_ONLY
        if (present(verbosity)) then
            verbosity_level = verbosity
        end if
        i_condition = merge(1_c_int, 0_c_int, condition)
        call c_assert_false(i_condition, verbosity_level)
    end subroutine assert_false

end module fortest_assert
