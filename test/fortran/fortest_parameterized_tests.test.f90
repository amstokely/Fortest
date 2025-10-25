!> @brief test_fortest_param_with_fixture Parameterized Test with Fixture
!>
!> This test suite demonstrates parameterized tests that use a fixture
!> with arrays. Each parameter index corresponds to an element in the
!> fixture arrays, allowing the same test logic to validate multiple
!> cases.

module test_fortest_parameterized_tests_mod
    use fortest_assert, only : assert_equal
    use iso_c_binding, only : c_ptr, c_f_pointer, c_int
    implicit none

    !> Fixture type storing arrays of expected values
    type :: param_fixture_t
        integer(c_int), allocatable :: expected(:)
        integer(c_int), allocatable :: upper_bound(:)
    end type param_fixture_t

contains

    !--- Setup/teardown for the fixture ---
    subroutine setup_param_fixture(f_ptr)
        type(c_ptr), value :: f_ptr
        type(param_fixture_t), pointer :: f
        call c_f_pointer(f_ptr, f)

        allocate(f%expected(3))
        allocate(f%upper_bound(3))

        f%expected = [0_c_int, 1_c_int, 2_c_int]
        f%upper_bound = [2_c_int, 2_c_int, 2_c_int]
    end subroutine setup_param_fixture

    subroutine teardown_param_fixture(f_ptr)
        type(c_ptr), value :: f_ptr
        type(param_fixture_t), pointer :: f
        call c_f_pointer(f_ptr, f)

        if (allocated(f%expected))    deallocate(f%expected)
        if (allocated(f%upper_bound)) deallocate(f%upper_bound)
    end subroutine teardown_param_fixture

    !--- Parameterized tests that use fixture arrays ---
    !> Check that the parameter index equals the expected(idx+1)
    subroutine test_param_fixture_expected(t_ptr, ts_ptr, s_ptr, idx) bind(C)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        integer(c_int), value :: idx
        type(param_fixture_t), pointer :: t
        call c_f_pointer(t_ptr, t)

        call assert_equal(idx, t%expected(idx + 1))
    end subroutine test_param_fixture_expected

    !> Check that the parameter index is <= upper_bound(idx+1)
    subroutine test_param_fixture_upper_bound(t_ptr, ts_ptr, s_ptr, idx) bind(C)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        integer(c_int), value :: idx
        type(param_fixture_t), pointer :: t
        call c_f_pointer(t_ptr, t)

        call assert_equal(idx <= t%upper_bound(idx + 1), .true.)
    end subroutine test_param_fixture_upper_bound

end module test_fortest_parameterized_tests_mod

!-------------------------------------------------------------
! Program: Register and run the parameterized fixture tests
!-------------------------------------------------------------
program test_fortest_parameterized_tests
    use fortest_test_session, only : test_session_t
    use test_fortest_parameterized_tests_mod, only : &
            param_fixture_t, &
            setup_param_fixture, teardown_param_fixture, &
            test_param_fixture_expected, test_param_fixture_upper_bound
    use iso_c_binding, only : c_ptr, c_loc, c_int
    implicit none

    type(test_session_t) :: test_session
    type(param_fixture_t), target :: fixture
    type(c_ptr) :: fixture_ptr
    integer(c_int), dimension(3) :: params = [0_c_int, 1_c_int, 2_c_int]

    fixture_ptr = c_loc(fixture)

    ! Register suite
    call test_session%register_test_suite("param_fixture_suite")

    ! Register fixture at test scope (new setup/teardown per test run)
    call test_session%register_fixture(&
            setup = setup_param_fixture, &
            teardown = teardown_param_fixture, &
            args = fixture_ptr, &
            scope = "test", &
            test_suite_name = "param_fixture_suite")

    ! Register parameterized tests
    call test_session%register_parameterized_test(&
            test_suite_name = "param_fixture_suite", &
            test_name = "test_expected_values", &
            test = test_param_fixture_expected, &
            params = params)

    ! Register parameterized tests
    call test_session%register_parameterized_test(&
            test_suite_name = "param_fixture_suite", &
            test_name = "test_expected_values", &
            test = test_param_fixture_expected, &
            num_params = 3)

    call test_session%register_parameterized_test(&
            test_suite_name = "param_fixture_suite", &
            test_name = "test_upper_bound_check", &
            test = test_param_fixture_upper_bound, &
            params = params)

    ! Run the session
    call test_session%run()
    call test_session%finalize()
end program
