!> @brief Unit tests for math operations using Fortest.
!>
!> @details
!> This module defines tests for:
!> - Matrix multiplication (`mat_mul`)
!> - Cross product (`cross_product`)
!> - Vector normalization (`normalize_vector`)
!>
!> Tests rely on fixtures provided by `math_ops_fixture`:
!> - Suite-level fixture holds constant matrices and vectors.
!> - Test-level fixture provides scratch space reset before each test.
module test_math_ops_mod
   use iso_c_binding, only: c_ptr, c_f_pointer
   use fortest_assert, only: assert_equal
   use math_ops_mod, only: mat_mul, cross_product, normalize_vector
   use math_ops_fixture
   implicit none
contains

   !> @test Basic matrix multiplication A*B, checking two entries.
   subroutine test_mat_mul_basic(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call mat_mul(ts%A, ts%B, t%C)
      call assert_equal(t%C(1, 1), 23.0)
      call assert_equal(t%C(2, 2), 46.0)
   end subroutine test_mat_mul_basic

   !> @test Full matrix multiplication A*B, checking all four entries.
   subroutine test_mat_mul_full(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call mat_mul(ts%A, ts%B, t%C)
      call assert_equal(t%C(1, 1), 23.0)
      call assert_equal(t%C(1, 2), 31.0)
      call assert_equal(t%C(2, 1), 34.0)
      call assert_equal(t%C(2, 2), 46.0)
   end subroutine test_mat_mul_full

   !> @test Multiplication by identity matrix leaves input unchanged.
   subroutine test_mat_mul_identity(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call mat_mul(ts%A, ts%I, t%C)
      call assert_equal(t%C(1, 1), ts%A(1, 1))
      call assert_equal(t%C(2, 1), ts%A(2, 1))
      call assert_equal(t%C(1, 2), ts%A(1, 2))
      call assert_equal(t%C(2, 2), ts%A(2, 2))
   end subroutine test_mat_mul_identity

   !> @test Multiplication by zero matrix yields all-zero result.
   subroutine test_mat_mul_zero(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call mat_mul(ts%A, ts%Z, t%C)
      call assert_equal(t%C(1, 1), 0.0)
      call assert_equal(t%C(1, 2), 0.0)
      call assert_equal(t%C(2, 1), 0.0)
      call assert_equal(t%C(2, 2), 0.0)
   end subroutine test_mat_mul_zero

   !> @test Cross product U × V matches expected reference vector.
   subroutine test_cross_product_expected(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call cross_product(ts%U, ts%V, t%W)

      call assert_equal(t%W(1), ts%W_expected(1))
      call assert_equal(t%W(2), ts%W_expected(2))
      call assert_equal(t%W(3), ts%W_expected(3))
   end subroutine test_cross_product_expected

   !> @test Cross product is anti-commutative: U×V = -(V×U).
   subroutine test_cross_product_anticommutative(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      real :: Wvu(3)

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call cross_product(ts%U, ts%V, t%W)
      call cross_product(ts%V, ts%U, Wvu)

      call assert_equal(Wvu(1), -t%W(1))
      call assert_equal(Wvu(2), -t%W(2))
      call assert_equal(Wvu(3), -t%W(3))
   end subroutine test_cross_product_anticommutative

   !> @test Cross product of parallel vectors should be zero vector.
   subroutine test_cross_product_parallel(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      real :: U2(3), V2(3)

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      U2 = [1.0, 2.0, 3.0]
      V2 = 2.0 * U2   ! parallel vector

      call cross_product(U2, V2, t%W)

      call assert_equal(t%W(1), 0.0)
      call assert_equal(t%W(2), 0.0)
      call assert_equal(t%W(3), 0.0)
   end subroutine test_cross_product_parallel

   !> @test Cross product result is perpendicular to both inputs.
   subroutine test_cross_product_perpendicularity(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      real :: dot_u, dot_v

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call cross_product(ts%U, ts%V, t%W)

      dot_u = sum(ts%U * t%W)
      dot_v = sum(ts%V * t%W)

      call assert_equal(dot_u, 0.0)
      call assert_equal(dot_v, 0.0)
   end subroutine test_cross_product_perpendicularity

   !> @test Normalized vector has unit length.
   subroutine test_normalize_vector_length(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      real :: U_norm(3), length

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call normalize_vector(ts%U, U_norm)
      length = sqrt(sum(U_norm * U_norm))

      call assert_equal(length, 1.0, abs_tol=1.0e-5)
   end subroutine test_normalize_vector_length

   !> @test Normalization preserves direction of vector U.
   subroutine test_normalize_vector_direction(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      real :: U_norm(3), ratio

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      call normalize_vector(ts%U, U_norm)

      ratio = U_norm(1) / ts%U(1) ! safe since U(1) /= 0
      call assert_equal(U_norm(2), ts%U(2) * ratio)
      call assert_equal(U_norm(3), ts%U(3) * ratio)
   end subroutine test_normalize_vector_direction

   !> @test Normalizing zero vector yields zero vector.
   subroutine test_normalize_zero_vector(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      real :: U0(3), U_norm(3)

      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)

      U0 = [0.0, 0.0, 0.0]
      call normalize_vector(U0, U_norm)

      call assert_equal(U_norm(1), 0.0)
      call assert_equal(U_norm(2), 0.0)
      call assert_equal(U_norm(3), 0.0)
   end subroutine test_normalize_zero_vector

end module test_math_ops_mod


!===========================================================
!> @brief Driver program to run math operations tests.
!>
!> @details
!> Initializes suite-level and test-level fixtures, registers
!> all matrix, cross product, and normalization tests with Fortest,
!> then executes the full test session.
program test_math_ops
   use iso_c_binding, only: c_ptr, c_loc
   use fortest_test_session, only: test_session_t
   use test_math_ops_mod
   use math_ops_fixture, only: suite_fixture_t, test_fixture_t, &
           setup_suite_fixture, teardown_suite_fixture, &
           setup_test_fixture, teardown_test_fixture
   implicit none

   type(test_session_t) :: session
   type(suite_fixture_t), target :: suite_fix
   type(test_fixture_t), target :: test_fix
   type(c_ptr) :: suite_ptr, test_ptr

   suite_ptr = c_loc(suite_fix)
   test_ptr  = c_loc(test_fix)

   call session%register_test_suite("math_ops")

   ! Register suite-level fixture
   call session%register_fixture( &
           setup = setup_suite_fixture, &
           teardown = teardown_suite_fixture, &
           args = suite_ptr, &
           scope = "suite", &
           test_suite_name = "math_ops")

   ! Register test-level fixture
   call session%register_fixture( &
           setup = setup_test_fixture, &
           teardown = teardown_test_fixture, &
           args = test_ptr, &
           scope = "test", &
           test_suite_name = "math_ops")

   ! Register all tests
   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_mat_mul_basic", &
         test            = test_mat_mul_basic)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_mat_mul_full", &
         test            = test_mat_mul_full)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_mat_mul_identity", &
         test            = test_mat_mul_identity)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_mat_mul_zero", &
         test            = test_mat_mul_zero)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_cross_product_expected", &
         test            = test_cross_product_expected)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_cross_product_anticommutative", &
         test            = test_cross_product_anticommutative)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_cross_product_parallel", &
         test            = test_cross_product_parallel)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_cross_product_perpendicularity", &
         test            = test_cross_product_perpendicularity)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_normalize_vector_length", &
         test            = test_normalize_vector_length)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_normalize_vector_direction", &
         test            = test_normalize_vector_direction)

   call session%register_test( &
         test_suite_name = "math_ops", &
         test_name       = "test_normalize_zero_vector", &
         test            = test_normalize_zero_vector)

   call session%run()
   call session%finalize()
end program test_math_ops
