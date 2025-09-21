!> @brief Tests for verifying test session finalization and status handling.
!>
!> This module defines passing and failing tests to ensure the
!> framework correctly aggregates results and reports failures.
module test_finalize_test_session_mod
   use iso_c_binding, only: c_ptr
   use fortest_assert, only: assert_equal, assert_not_equal, assert_true, assert_false
   implicit none
contains

   !> @test Verify that addition passes (2 + 3 = 5).
   subroutine test_add_passes(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      integer :: a, b, result
      a = 2
      b = 3
      result = a + b
      call assert_equal(result, 5)
   end subroutine test_add_passes

   !> @test Verify that subtraction passes (3 − 2 = 1).
   subroutine test_subtract_passes(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      integer :: a, b, result
      a = 2
      b = 3
      result = b - a
      call assert_equal(result, 1)
   end subroutine test_subtract_passes

   !> @test Deliberate failure: addition result (2 + 3 = 5) checked against 6.
   subroutine test_add_fails(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      integer :: a, b, result
      a = 2
      b = 3
      result = a + b
      call assert_equal(result, 6)  ! This will fail
   end subroutine test_add_fails

   !> @test Deliberate failure: subtraction result (3 − 2 = 1) checked against 0.
   subroutine test_subtract_fails(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      integer :: a, b, result
      a = 2
      b = 3
      result = b - a
      call assert_equal(result, 0)  ! This will fail
   end subroutine test_subtract_fails

end module test_finalize_test_session_mod


!> @brief Driver program for test session finalization checks.
!>
!> This program:
!> 1. Runs passing tests and checks for zero status.
!> 2. Runs failing tests and checks that status is non-zero.
!> 3. Exits with failure if the expected outcome is not observed.
program test_assert_no_fixture
   use test_finalize_test_session_mod, only: test_add_passes, test_subtract_passes, test_add_fails, test_subtract_fails
   use fortest_test_session
   implicit none
   type(test_suite_t)   :: test_suite
   type(test_session_t) :: test_session

   call test_session%register_test_suite("test_finalize_test_session")

   call test_session%register_test("test_finalize_test_session", "test_add_passes", test_add_passes)
   call test_session%run()
   if (test_session%get_status() /= 0) then
      call exit(1)
   end if

   call test_session%register_test("test_finalize_test_session", "test_subtract_passes", test_subtract_passes)
   call test_session%run()
   if (test_session%get_status() /= 0) then
      call exit(1)
   end if

   call test_session%register_test("test_finalize_test_session", "test_add_fails", test_add_fails)
   call test_session%run()
   if (test_session%get_status() == 0) then
      call exit(1)
   end if

   call test_session%register_test("test_finalize_test_session", "test_subtract_fails", test_subtract_fails)
   call test_session%run()
   if (test_session%get_status() == 0) then
      call exit(1)
   end if

end program test_assert_no_fixture
