!> @brief Tests for the f_c_string_t type and its C string conversions.
!>
!> This module defines unit tests that verify:
!> - Calling `to_c` before initialization fails
!> - Calling `to_c` twice fails
!> - Converting "abc" works correctly with the C++ side
!> - Converting "abc def" works correctly with the C++ side
module test_f_c_string_t_mod
   use iso_c_binding, only: c_ptr
   use f_c_string_t_mod
   use fortest_assert, only: assert_equal, assert_not_equal
   implicit none

   !> @brief C interface to a function that checks if a C string equals "abc".
   interface
      function str_equals_abc(c_string) bind(C, name = "str_equals_abc")
         import :: c_ptr
         type(c_ptr), value :: c_string
         integer :: str_equals_abc
      end function str_equals_abc

      !> @brief C interface to a function that checks if a C string equals "abc def".
      function str_equals_abc_space_def(c_string) bind(C, name = "str_equals_abc_space_def")
         import :: c_ptr
         type(c_ptr), value :: c_string
         integer :: str_equals_abc_space_def
      end function str_equals_abc_space_def
   end interface

contains

   !> @test Verify that calling `to_c` before initialization fails.
   subroutine test_f_c_string_t_call_to_c_before_init(t_ptr, ts_ptr, s_ptr)
      type (c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(f_c_string_t) :: abc
      integer :: status

      status = abc%to_c()
      call assert_not_equal(status, 0, verbosity=2)
   end subroutine

   !> @test Verify that calling `to_c` twice fails.
   subroutine test_f_c_string_t_double_c_memcpy(t_ptr, ts_ptr, s_ptr)
      type (c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(f_c_string_t) :: abc
      integer :: status

      abc = f_c_string_t("abc")
      status = abc%to_c()
      status = abc%to_c()
      call assert_not_equal(status, 0, verbosity=2)
   end subroutine

   !> @test Convert "abc" to C string and check with C++ side.
   subroutine test_f_c_string_t_no_spaces(t_ptr, ts_ptr, s_ptr)
      type (c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(f_c_string_t) :: abc
      integer :: status

      abc = f_c_string_t("abc")
      status = abc%to_c()
      call assert_equal(status, 0, verbosity=2)
      status = str_equals_abc(abc%get_c_string())
      call assert_equal(status, 0, verbosity=2)
   end subroutine

   !> @test Convert "abc def" to C string and check with C++ side.
   subroutine test_f_c_string_t_with_spaces(t_ptr, ts_ptr, s_ptr)
      type (c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(f_c_string_t) :: abc_def
      integer :: status

      abc_def = f_c_string_t("abc def")
      status = abc_def%to_c()
      call assert_equal(status, 0, verbosity=2)
      status = str_equals_abc_space_def(abc_def%get_c_string())
      call assert_equal(status, 0, verbosity=2)
   end subroutine

end module test_f_c_string_t_mod

!> @brief Test driver for f_c_string_t conversion and validation.
program test_f_c_string_t
   use fortest_test_session, only: test_session_t
   use test_f_c_string_t_mod
   implicit none
   type(test_session_t) :: test_session

   call test_session%register_test_suite("test_f_c_string_t")

   call test_session%register_test(&
         test_suite_name = "test_f_c_string_t", &
         test_name = "call_to_c_before_init", &
         test = test_f_c_string_t_call_to_c_before_init)

   call test_session%register_test(&
         test_suite_name = "test_f_c_string_t", &
         test_name = "double_c_memcpy", &
         test = test_f_c_string_t_double_c_memcpy)

   call test_session%register_test(&
         test_suite_name = "test_f_c_string_t", &
         test_name = "no_spaces", &
         test = test_f_c_string_t_no_spaces)

   call test_session%register_test(&
         test_suite_name = "test_f_c_string_t", &
         test_name = "with_spaces", &
         test = test_f_c_string_t_with_spaces)

   call test_session%run()
   call test_session%finalize()
end program test_f_c_string_t
