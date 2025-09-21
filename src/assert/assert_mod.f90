!> @brief Assertion utilities for Fortran tests.
!>
!> @details
!> This module provides typed assertion routines that wrap C
!> implementations for consistency and integration with the Fortest
!> framework. Supported assertions include:
!> - Equality and inequality for integers, reals, doubles, and strings
!> - Boolean checks (`assert_true`, `assert_false`)
!>
!> These routines are thin Fortran wrappers that forward arguments to
!> corresponding C functions for logging and reporting.
module fortest_assert
   use iso_c_binding, only: c_int, c_float, c_double, c_ptr
   use f_c_string_t_mod, only: f_c_string_t

   implicit none
   private

   public :: assert_equal
   public :: assert_not_equal
   public :: assert_true
   public :: assert_false

   !> @brief Assert equality of two values.
   interface assert_equal
      module procedure assert_equal_int
      module procedure assert_equal_float
      module procedure assert_equal_double
      module procedure assert_equal_string
   end interface assert_equal

   !> @brief Assert inequality of two values.
   interface assert_not_equal
      module procedure assert_not_equal_int
      module procedure assert_not_equal_float
      module procedure assert_not_equal_double
      module procedure assert_not_equal_string
   end interface assert_not_equal

contains

   !> @brief Assert that two integers are equal.
   !> @param expected Expected integer value.
   !> @param actual   Actual integer value.
   subroutine assert_equal_int(expected, actual)
      integer(c_int), intent(in) :: expected, actual
      interface
         subroutine c_assert_equal_int(expected, actual) bind(C)
            import :: c_int
            integer(c_int), value :: expected, actual
         end subroutine c_assert_equal_int
      end interface
      call c_assert_equal_int(expected, actual)
   end subroutine assert_equal_int

   !> @brief Assert that two real (single precision) values are equal.
   !> Supports absolute and relative tolerance.
   !> @param expected Expected real value.
   !> @param actual   Actual real value.
   !> @param abs_tol  Absolute tolerance (optional).
   !> @param rel_tol  Relative tolerance (optional).
   subroutine assert_equal_float(expected, actual, abs_tol, rel_tol)
      use iso_c_binding, only: c_float, c_double
      real(c_float), intent(in) :: expected, actual
      real(c_float), intent(in), optional :: abs_tol, rel_tol
      real(c_float) :: a_tol, r_tol

      interface
         subroutine c_assert_equal_float(expected, actual, abs_tol, rel_tol) bind(C)
            import :: c_float, c_double
            real(c_float), value :: expected, actual
            real(c_float), value :: abs_tol, rel_tol
         end subroutine c_assert_equal_float
      end interface

      ! Default tolerances to zero if not provided
      a_tol = 0.0
      r_tol = 0.0
      if (present(abs_tol)) a_tol = abs_tol
      if (present(rel_tol)) r_tol = rel_tol

      call c_assert_equal_float(expected, actual, a_tol, r_tol)
   end subroutine assert_equal_float


   !> @brief Assert that two double precision values are equal.
   !> Supports absolute and relative tolerance.
   !> @param expected Expected double precision value.
   !> @param actual   Actual double precision value.
   !> @param abs_tol  Absolute tolerance (optional).
   !> @param rel_tol  Relative tolerance (optional).
   subroutine assert_equal_double(expected, actual, abs_tol, rel_tol)
      use iso_c_binding, only: c_double
      real(c_double), intent(in) :: expected, actual
      real(c_double), intent(in), optional :: abs_tol, rel_tol
      real(c_double) :: a_tol, r_tol

      interface
         subroutine c_assert_equal_double(expected, actual, abs_tol, rel_tol) bind(C)
            import :: c_double
            real(c_double), value :: expected, actual
            real(c_double), value :: abs_tol, rel_tol
         end subroutine c_assert_equal_double
      end interface

      ! Default tolerances to zero if not provided
      a_tol = 0.0d0
      r_tol = 0.0d0
      if (present(abs_tol)) a_tol = abs_tol
      if (present(rel_tol)) r_tol = rel_tol

      call c_assert_equal_double(expected, actual, a_tol, r_tol)
   end subroutine assert_equal_double


   !> @brief Assert that two character strings are equal.
   !> @param expected Expected string value.
   !> @param actual   Actual string value.
   subroutine assert_equal_string(expected, actual)
      character(len = *), intent(in) :: expected, actual
      type(f_c_string_t) :: f_c_string_expected, f_c_string_actual
      integer :: status
      interface
         subroutine c_assert_equal_string(expected, actual) bind(C)
            import :: c_ptr
            type(c_ptr), value, intent(in) :: expected, actual
         end subroutine c_assert_equal_string
      end interface

      f_c_string_expected = f_c_string_t(expected)
      f_c_string_actual = f_c_string_t(actual)
      status = f_c_string_expected%to_c()
      status = f_c_string_actual%to_c()
      call c_assert_equal_string(&
            f_c_string_expected%get_c_string(), &
            f_c_string_actual%get_c_string())
   end subroutine assert_equal_string

   !> @brief Assert that two integers are not equal.
   !> @param expected Integer value to compare.
   !> @param actual   Integer value to compare.
   subroutine assert_not_equal_int(expected, actual)
      integer(c_int), intent(in) :: expected, actual
      interface
         subroutine c_assert_not_equal_int(expected, actual) bind(C)
            import :: c_int
            integer(c_int), value :: expected, actual
         end subroutine c_assert_not_equal_int
      end interface
      call c_assert_not_equal_int(expected, actual)
   end subroutine assert_not_equal_int

   !> @brief Assert that two real (single precision) values are not equal.
   !> Supports absolute and relative tolerance.
   !> @param expected Real value to compare.
   !> @param actual   Real value to compare.
   !> @param abs_tol  Absolute tolerance.
   !> @param rel_tol  Relative tolerance.
   subroutine assert_not_equal_float(expected, actual, abs_tol, rel_tol)
      use iso_c_binding, only: c_float, c_double
      real(c_float), intent(in) :: expected, actual
      real(c_float), intent(in), optional :: abs_tol, rel_tol
      real(c_float) :: a_tol, r_tol

      interface
         subroutine c_assert_not_equal_float(expected, actual, abs_tol, rel_tol) bind(C)
            import :: c_float, c_double
            real(c_float), value :: expected, actual
            real(c_float), value :: abs_tol, rel_tol
         end subroutine c_assert_not_equal_float
      end interface

      ! Default tolerances to zero if not provided
      a_tol = 0.0_c_float
      r_tol = 0.0_c_float
      if (present(abs_tol)) a_tol = abs_tol
      if (present(rel_tol)) r_tol = rel_tol

      call c_assert_not_equal_float(expected, actual, a_tol, r_tol)
   end subroutine assert_not_equal_float


   !> @brief Assert that two double precision values are not equal.
   !> Supports absolute and relative tolerance.
   !> @param expected Double precision value to compare.
   !> @param actual   Double precision value to compare.
   !> @param abs_tol  Absolute tolerance.
   !> @param rel_tol  Relative tolerance.
   subroutine assert_not_equal_double(expected, actual, abs_tol, rel_tol)
      use iso_c_binding, only: c_double
      real(c_double), intent(in) :: expected, actual
      real(c_double), intent(in), optional :: abs_tol, rel_tol
      real(c_double) :: a_tol, r_tol

      interface
         subroutine c_assert_not_equal_double(expected, actual, abs_tol, rel_tol) bind(C)
            import :: c_double
            real(c_double), value :: expected, actual
            real(c_double), value :: abs_tol, rel_tol
         end subroutine c_assert_not_equal_double
      end interface

      ! Default tolerances to zero if not provided
      a_tol = 0.0d0
      r_tol = 0.0d0
      if (present(abs_tol)) a_tol = abs_tol
      if (present(rel_tol)) r_tol = rel_tol

      call c_assert_not_equal_double(expected, actual, a_tol, r_tol)
   end subroutine assert_not_equal_double


   !> @brief Assert that two character strings are not equal.
   !> @param expected String value to compare.
   !> @param actual   String value to compare.
   subroutine assert_not_equal_string(expected, actual)
      character(len = *), intent(in) :: expected, actual
      type(f_c_string_t) :: f_c_string_expected, f_c_string_actual
      integer :: status
      interface
         subroutine c_assert_not_equal_string(expected, actual) bind(C)
            import :: c_ptr
            type(c_ptr), value, intent(in) :: expected, actual
         end subroutine c_assert_not_equal_string
      end interface

      f_c_string_expected = f_c_string_t(expected)
      f_c_string_actual = f_c_string_t(actual)
      status = f_c_string_expected%to_c()
      status = f_c_string_actual%to_c()
      call c_assert_not_equal_string(&
            f_c_string_expected%get_c_string(), &
            f_c_string_actual%get_c_string())
   end subroutine assert_not_equal_string

   !> @brief Assert that a logical condition is true.
   !> @param condition Logical condition to check.
   subroutine assert_true(condition)
      logical, intent(in) :: condition
      integer(c_int) :: i_condition
      interface
         subroutine c_assert_true(condition) bind(C, name = "c_assert_true")
            import :: c_int
            integer(c_int), value :: condition
         end subroutine c_assert_true
      end interface
      i_condition = merge(1_c_int, 0_c_int, condition)
      call c_assert_true(i_condition)
   end subroutine assert_true

   !> @brief Assert that a logical condition is false.
   !> @param condition Logical condition to check.
   subroutine assert_false(condition)
      logical, intent(in) :: condition
      integer(c_int) :: i_condition
      interface
         subroutine c_assert_false(condition) bind(C, name = "c_assert_false")
            import :: c_int
            integer(c_int), value :: condition
         end subroutine c_assert_false
      end interface
      i_condition = merge(1_c_int, 0_c_int, condition)
      call c_assert_false(i_condition)
   end subroutine assert_false

end module fortest_assert
