!> @brief Tests for assert routines without fixtures.
!>
!> @details
!> This module defines standalone tests that directly exercise
!> `assert_equal`, `assert_not_equal`, `assert_true`, and
!> `assert_false`. Unlike fixture-based tests, no shared state is
!> initialized or reset between runs.
module test_assert_no_fixture_mod
    use iso_c_binding, only : c_ptr
    use fortest_assert, only : assert_equal, assert_not_equal, assert_true, assert_false
    implicit none
contains

    !> @test Verify that 2 + 3 = 5 (integer equality).
    subroutine test_assert_equal_int(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        integer :: a, b, result
        a = 2
        b = 3
        result = a + b
        call assert_equal(result, 5)
    end subroutine test_assert_equal_int

    !> @test Verify that .true. = .true. and .false = .false (logical equality).
    subroutine test_assert_equal_logical(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        logical :: val1, val2
        val1 = .true.
        val2 = .true.
        call assert_equal(val1, val2)
        val1 = .false.
        val2 = .false.
        call assert_equal(val1, val2)
    end subroutine test_assert_equal_logical

    !> @test Verify that 1.5 × 2.5 = 3.75 (real equality).
    subroutine test_assert_equal_float(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        real :: x, y, result
        x = 1.5
        y = 2.5
        result = x * y
        call assert_equal(result, 3.75)
    end subroutine test_assert_equal_float

    !> @test Verify real equality with absolute tolerance.
    subroutine test_assert_equal_float_abs_tol(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        real :: val, approx
        val    = 1.0
        approx = 1.0001
        call assert_equal(val, approx, abs_tol=1.0e-3)
    end subroutine test_assert_equal_float_abs_tol

    !> @test Verify real equality with relative tolerance.
    subroutine test_assert_equal_float_rel_tol(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        real :: val, approx
        val    = 1000.0
        approx = 1001.0
        call assert_equal(val, approx, rel_tol=1.0e-2) ! within 1%
    end subroutine test_assert_equal_float_rel_tol

    !> @test Verify that 1.5d0 × 2.5d0 = 3.75d0 (double equality).
    subroutine test_assert_equal_double(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        double precision :: x, y, result
        x = 1.5d0
        y = 2.5d0
        result = x * y
        call assert_equal(result, 3.75d0)
    end subroutine test_assert_equal_double

    !> @test Verify double equality with absolute tolerance.
    subroutine test_assert_equal_double_abs_tol(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        double precision :: val, approx
        val    = 2.0d0
        approx = 2.000001d0
        call assert_equal(val, approx, abs_tol=1.0d-5)
    end subroutine test_assert_equal_double_abs_tol

    !> @test Verify double equality with relative tolerance.
    subroutine test_assert_equal_double_rel_tol(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        double precision :: val, approx
        val    = 5000.0d0
        approx = 5005.0d0
        call assert_equal(val, approx, rel_tol=1.0d-2) ! within 1%
    end subroutine test_assert_equal_double_rel_tol

    !> @test Verify that "Hello" // " " // "World" = "Hello World" (string equality).
    subroutine test_assert_equal_string(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        character(len = 20) :: str1, str2
        str1 = "Hello"
        str2 = "World"
        call assert_equal(trim(str1) // " " // trim(str2), "Hello World")
    end subroutine test_assert_equal_string

    !> @test Verify that 2 + 3 ≠ 4 (integer inequality).
    subroutine test_assert_not_equal_int(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        integer :: a, b, result
        a = 2
        b = 3
        result = a + b
        call assert_not_equal(result, 4)
    end subroutine test_assert_not_equal_int

    !> @test Verify .true. ≠ .false. (logical inequality).
    subroutine test_assert_not_equal_logical(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        logical :: val1, val2
        val1 = .true.
        val2 = .false.
        call assert_not_equal(val1, val2)
    end subroutine test_assert_not_equal_logical

    !> @test Verify that 1.5 × 2.5 = 3.75 ≠ 4.0 (real inequality).
    subroutine test_assert_not_equal_float(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        real :: x, y, result
        x = 1.5
        y = 2.5
        result = x * y
        call assert_not_equal(result, 4.0)
    end subroutine test_assert_not_equal_float

    !> @test Verify real inequality outside tolerance.
    subroutine test_assert_not_equal_float_tol(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        real :: val, approx
        val    = 1.0
        approx = 1.2
        call assert_not_equal(val, approx, abs_tol=1.0e-2)
    end subroutine test_assert_not_equal_float_tol

    !> @test Verify that 1.5d0 × 2.5d0 = 3.75d0 ≠ 4.0d0 (double inequality).
    subroutine test_assert_not_equal_double(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        double precision :: x, y, result
        x = 1.5d0
        y = 2.5d0
        result = x * y
        call assert_not_equal(result, 4.0d0)
    end subroutine test_assert_not_equal_double

    !> @test Verify double inequality outside tolerance.
    subroutine test_assert_not_equal_double_tol(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        double precision :: val, approx
        val    = 10.0d0
        approx = 10.5d0
        call assert_not_equal(val, approx, rel_tol=1.0d-2)
    end subroutine test_assert_not_equal_double_tol

    !> @test Verify that "Hello World" ≠ "Goodbye World" (string inequality).
    subroutine test_assert_not_equal_string(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        character(len = 20) :: str1, str2
        str1 = "Hello"
        str2 = "World"
        call assert_not_equal(trim(str1) // " " // trim(str2), "Goodbye World")
    end subroutine test_assert_not_equal_string

    !> @test Verify that (2 + 2 == 4) evaluates to .true.
    subroutine test_assert_true(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        logical :: condition
        condition = (2 + 2 == 4)
        call assert_true(condition)
    end subroutine test_assert_true

    !> @test Verify that (2 + 2 == 5) evaluates to .false.
    subroutine test_assert_false(t_ptr, ts_ptr, s_ptr)
        type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
        logical :: condition
        condition = (2 + 2 == 5)
        call assert_false(condition)
    end subroutine test_assert_false

end module test_assert_no_fixture_mod


!> @brief Driver program to run all no-fixture assert tests.
program test_assert_no_fixture
    use test_assert_no_fixture_mod
    use fortest_test_session
    implicit none
    type(test_suite_t)   :: test_suite
    type(test_session_t) :: test_session

    call test_session%register_test_suite("test_suite")

    ! Equality tests
    call test_session%register_test("test_suite", "test_assert_equal_int", test_assert_equal_int)
    call test_session%register_test("test_suite", "test_assert_equal_float", test_assert_equal_float)
    call test_session%register_test("test_suite", "test_assert_equal_float_abs_tol", test_assert_equal_float_abs_tol)
    call test_session%register_test("test_suite", "test_assert_equal_float_rel_tol", test_assert_equal_float_rel_tol)
    call test_session%register_test("test_suite", "test_assert_equal_double", test_assert_equal_double)
    call test_session%register_test("test_suite", "test_assert_equal_double_abs_tol", test_assert_equal_double_abs_tol)
    call test_session%register_test("test_suite", "test_assert_equal_double_rel_tol", test_assert_equal_double_rel_tol)
    call test_session%register_test("test_suite", "test_assert_equal_string", test_assert_equal_string)

    ! Inequality tests
    call test_session%register_test("test_suite", "test_assert_not_equal_int", test_assert_not_equal_int)
    call test_session%register_test("test_suite", "test_assert_not_equal_float", test_assert_not_equal_float)
    call test_session%register_test("test_suite", "test_assert_not_equal_float_tol", test_assert_not_equal_float_tol)
    call test_session%register_test("test_suite", "test_assert_not_equal_double", test_assert_not_equal_double)
    call test_session%register_test("test_suite", "test_assert_not_equal_double_tol", test_assert_not_equal_double_tol)
    call test_session%register_test("test_suite", "test_assert_not_equal_string", test_assert_not_equal_string)

    ! Logical tests
    call test_session%register_test("test_suite", "test_assert_true", test_assert_true)
    call test_session%register_test("test_suite", "test_assert_false", test_assert_false)

    call test_session%run()
    call test_session%finalize()
end program test_assert_no_fixture
