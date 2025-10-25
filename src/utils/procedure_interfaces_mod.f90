module procedure_interfaces_mod
    use iso_c_binding
    implicit none
    private
    public :: test_proc, fixture_proc, param_test_proc

    !> @brief Abstract interface for a test procedure.
    !!
    !! A test procedure is a subroutine with three C pointer
    !! arguments:
    !! - t_ptr  : pointer to the test instance
    !! - ts_ptr : pointer to the test suite
    !! - s_ptr  : pointer to the test session
    !!
    !! These pointers allow the Fortran procedure to interact
    !! with the underlying C++ test management system.
    abstract interface
        subroutine test_proc(t_ptr, ts_ptr, s_ptr)
            import :: c_ptr
            type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        end subroutine test_proc
    end interface

    !> @brief Abstract interface for a parameterized test procedure.
    !!
    !! A parameterized test procedure is a subroutine with three C pointer
    !! arguments and one integer index:
    !! - t_ptr  : pointer to the test instance
    !! - ts_ptr : pointer to the test suite
    !! - s_ptr  : pointer to the test session
    !! - idx    : integer parameter index for selecting values
    !!
    !! This allows the same test logic to run across multiple parameter values.
    abstract interface
        subroutine param_test_proc(t_ptr, ts_ptr, s_ptr, idx)
            import :: c_ptr, c_int
            type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
            integer(c_int), value :: idx
        end subroutine param_test_proc
    end interface


    !> @brief Abstract interface for a fixture procedure.
    !!
    !! A fixture procedure is a subroutine that takes a single
    !! C pointer argument:
    !! - args : pointer to fixture-specific arguments
    !!
    !! Used for setup and teardown routines in Fortran tests.
    abstract interface
        subroutine fixture_proc(args)
            import :: c_ptr
            type(c_ptr), value :: args
        end subroutine fixture_proc
    end interface

end module procedure_interfaces_mod
