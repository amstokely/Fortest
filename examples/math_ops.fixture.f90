!> @brief Math operations fixture module for Fortest.
!>
!> @details
!> Provides suite-level and test-level fixtures for testing
!> matrix and vector operations. The suite fixture contains
!> constant data such as matrices and vectors used across all tests,
!> while the test fixture provides scratch space that is reset
!> before each individual test.
module math_ops_fixture
   use iso_c_binding, only: c_ptr, c_f_pointer
   implicit none
   public :: suite_fixture_t, test_fixture_t
   public :: setup_suite_fixture, teardown_suite_fixture
   public :: setup_test_fixture, teardown_test_fixture

   !> @brief Suite-level fixture holding constants shared across all tests.
   !>
   !> @details
   !> Includes:
   !> - A, B: 2×2 test matrices
   !> - I: identity matrix
   !> - Z: zero matrix
   !> - U, V: 3-element test vectors
   !> - W_expected: expected result of U × V
   type :: suite_fixture_t
      real, allocatable :: A(:, :), B(:, :), I(:, :), Z(:, :)
      real, allocatable :: U(:), V(:), W_expected(:)
   end type suite_fixture_t

   !> @brief Test-level fixture providing scratch space.
   !>
   !> @details
   !> Reset before each test run. Contains:
   !> - C: 2×2 scratch matrix
   !> - W: 3-element scratch vector
   type :: test_fixture_t
      real, allocatable :: C(:, :)
      real, allocatable :: W(:)
   end type test_fixture_t

contains

   !> @brief Setup routine for test-level fixture.
   !>
   !> @param args C pointer to a test_fixture_t instance.
   !>
   !> Allocates a 2×2 scratch matrix `C` and a 3-element vector `W`,
   !> initializing them to zero.
   subroutine setup_test_fixture(args)
      type(c_ptr), value :: args
      type(test_fixture_t), pointer :: fix
      call c_f_pointer(args, fix)

      allocate(fix%C(2, 2))
      fix%C = 0.0

      allocate(fix%W(3))
      fix%W = 0.0
   end subroutine setup_test_fixture

   !> @brief Teardown routine for test-level fixture.
   !>
   !> @param args C pointer to a test_fixture_t instance.
   !>
   !> Deallocates the scratch matrix `C` and vector `W`.
   subroutine teardown_test_fixture(args)
      type(c_ptr), value :: args
      type(test_fixture_t), pointer :: fix
      call c_f_pointer(args, fix)

      if (allocated(fix%C)) deallocate(fix%C)
      if (allocated(fix%W)) deallocate(fix%W)
   end subroutine teardown_test_fixture

   !> @brief Setup routine for suite-level fixture.
   !>
   !> @param args C pointer to a suite_fixture_t instance.
   !>
   !> Allocates and initializes constant test data:
   !> - Matrices A and B with fixed values
   !> - Identity matrix I
   !> - Zero matrix Z
   !> - Vectors U and V
   !> - Expected cross product result W_expected
   subroutine setup_suite_fixture(args)
      type(c_ptr), value :: args
      type(suite_fixture_t), pointer :: fix
      call c_f_pointer(args, fix)

      ! Allocate matrices
      allocate(fix%A(2, 2), fix%B(2, 2), fix%I(2, 2), fix%Z(2, 2))
      fix%A = reshape([1.0, 2.0, 3.0, 4.0], [2, 2])
      fix%B = reshape([5.0, 6.0, 7.0, 8.0], [2, 2])

      fix%I = 0.0
      fix%I(1, 1) = 1.0
      fix%I(2, 2) = 1.0
      fix%Z = 0.0

      ! Allocate vectors
      allocate(fix%U(3), fix%V(3), fix%W_expected(3))
      fix%U = [1.0, 2.0, 3.0]
      fix%V = [4.0, 5.0, 6.0]
      fix%W_expected = [-3.0, 6.0, -3.0] ! U x V
   end subroutine setup_suite_fixture

   !> @brief Teardown routine for suite-level fixture.
   !>
   !> @param args C pointer to a suite_fixture_t instance.
   !>
   !> Deallocates all matrices and vectors from the suite fixture.
   subroutine teardown_suite_fixture(args)
      type(c_ptr), value :: args
      type(suite_fixture_t), pointer :: fix
      call c_f_pointer(args, fix)

      if (allocated(fix%A)) deallocate(fix%A)
      if (allocated(fix%B)) deallocate(fix%B)
      if (allocated(fix%I)) deallocate(fix%I)
      if (allocated(fix%Z)) deallocate(fix%Z)

      if (allocated(fix%U)) deallocate(fix%U)
      if (allocated(fix%V)) deallocate(fix%V)
      if (allocated(fix%W_expected)) deallocate(fix%W_expected)
   end subroutine teardown_suite_fixture

end module math_ops_fixture
