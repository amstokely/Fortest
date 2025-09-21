module fortest_test_suite
   use iso_c_binding
   use procedure_interfaces_mod, only: test_proc, fixture_proc
   implicit none

   public :: test_suite_t

   !> @brief Represents a test suite in Fortest.
   !!
   !! A test suite groups related tests under a common name.
   !! It allows registering fixtures with a specific scope,
   !! querying suite status, and finalizing (exiting with
   !! the suite result code).
   type test_suite_t
      !> Name of the test suite.
      character(len = :), allocatable :: name
   contains
      !> Retrieve the current status code of the suite.
      procedure :: get_status => get_status
      !> Finalize the suite and exit with its status code.
      procedure, public :: finalize
   end type test_suite_t

contains

   !> @brief Get the status code of this test suite.
   !!
   !! Calls into the C backend to fetch the most recent
   !! pass/fail status for the suite.
   !!
   !! @param[in] this The test suite.
   !! @return Status code (0 = success, nonzero = failure).
   function get_status(this) result(status)
      use iso_c_binding
      use f_c_string_t_mod, only: f_c_string_t
      implicit none
      class(test_suite_t), intent(in) :: this
      integer :: status, ierr
      type (f_c_string_t) :: f_c_string_name
      interface
         function c_get_test_suite_status(name) bind(C, name = "c_get_test_suite_status")
            import :: c_ptr
            type(c_ptr), value :: name
            integer :: c_get_test_suite_status
         end function c_get_test_suite_status
      end interface
      f_c_string_name = f_c_string_t(this%name)
      ierr = f_c_string_name%to_c()
      status = c_get_test_suite_status(f_c_string_name%get_c_string())
   end function get_status

   !> @brief Finalize this test suite and exit with its status code.
   !!
   !! This will terminate the program using the exit code
   !! returned by get_status(). A nonzero value indicates failure.
   subroutine finalize(this)
      implicit none
      class(test_suite_t), intent(in) :: this
      integer :: status

      status = this%get_status()
      call exit(status)
   end subroutine finalize

end module fortest_test_suite
