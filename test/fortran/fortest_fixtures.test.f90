module test_fortest_fixtures_mod
   use fortest_assert, only: assert_equal
   use iso_c_binding, only: c_ptr, c_f_pointer
   implicit none

   !> Fixture that resets on every test
   type :: test_fixture_t
      integer :: x = 0
   end type test_fixture_t

   !> Fixture that persists for all tests in a suite
   type :: suite_fixture_t
      integer :: x = 0
   end type suite_fixture_t

   !> Fixture that persists for the entire session
   type :: session_fixture_t
      integer :: x = 0
   end type session_fixture_t

contains
   ! --- Setup/teardown routines ---
   subroutine setup_test_fixture(f_ptr)
      type(c_ptr), value :: f_ptr
      type(test_fixture_t), pointer :: f
      call c_f_pointer(f_ptr, f)
      f%x = f%x + 1
   end subroutine

   subroutine teardown_test_fixture(f_ptr)
      type(c_ptr), value :: f_ptr
      type(test_fixture_t), pointer :: f
   end subroutine

   subroutine setup_suite_fixture(f_ptr)
      type(c_ptr), value :: f_ptr
      type(suite_fixture_t), pointer :: f
      call c_f_pointer(f_ptr, f)
      f%x = f%x + 1
   end subroutine

   subroutine teardown_suite_fixture(f_ptr)
      type(c_ptr), value :: f_ptr
      type(suite_fixture_t), pointer :: f
   end subroutine

   subroutine setup_session_fixture(f_ptr)
      type(c_ptr), value :: f_ptr
      type(session_fixture_t), pointer :: f
      call c_f_pointer(f_ptr, f)
      f%x = f%x + 1
   end subroutine

   subroutine teardown_session_fixture(f_ptr)
      type(c_ptr), value :: f_ptr
      type(session_fixture_t), pointer :: f
   end subroutine

   ! --- Tests for session-only suite ---
   !> Verify that the session fixture is initialized once and visible across all tests
   subroutine test_session_fixture_visible(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(session_fixture_t), pointer :: s
      call c_f_pointer(s_ptr, s)
      call assert_equal(s%x, 1)
   end subroutine

   ! --- Tests for suite fixture behavior ---
   !> Verify that the suite fixture is shared across all tests in a suite
   subroutine test_suite_fixture_shared(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(suite_fixture_t), pointer :: ts
      type(session_fixture_t), pointer :: s
      call c_f_pointer(ts_ptr, ts)
      call c_f_pointer(s_ptr, s)
      call assert_equal(ts%x, 1)
      call assert_equal(s%x, 1)
   end subroutine

   ! --- Tests for combined suite + test fixture behavior ---
   !> Verify that the test fixture is reset for the first test and the suite fixture persists
   subroutine test_first_test_fixture_reset(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      type(session_fixture_t), pointer :: s
      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)
      call c_f_pointer(s_ptr, s)
      call assert_equal(t%x, 1)
      call assert_equal(ts%x, 2)
      call assert_equal(s%x, 1)
   end subroutine

   !> Verify that the test fixture increments on the second test and the suite fixture persists
   subroutine test_second_test_fixture_reset(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      type(session_fixture_t), pointer :: s
      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)
      call c_f_pointer(s_ptr, s)
      call assert_equal(t%x, 2)
      call assert_equal(ts%x, 2)
      call assert_equal(s%x, 1)
   end subroutine

   !> Verify that the test fixture increments on the third test and the suite fixture persists
   subroutine test_third_test_fixture_reset(t_ptr, ts_ptr, s_ptr)
      type(c_ptr), value :: t_ptr, ts_ptr, s_ptr
      type(test_fixture_t), pointer :: t
      type(suite_fixture_t), pointer :: ts
      type(session_fixture_t), pointer :: s
      call c_f_pointer(t_ptr, t)
      call c_f_pointer(ts_ptr, ts)
      call c_f_pointer(s_ptr, s)
      call assert_equal(t%x, 3)
      call assert_equal(ts%x, 2)
      call assert_equal(s%x, 1)
   end subroutine

end module test_fortest_fixtures_mod

program test_fortest_fixtures
   use fortest_test_session, only: test_session_t
   use test_fortest_fixtures_mod
   use iso_c_binding, only: c_ptr, c_loc
   implicit none

   !> This test program verifies fixture scoping rules:
   !! - Session fixtures initialize once per session and persist everywhere.
   !! - Suite fixtures initialize once per suite and persist across all tests in that suite.
   !! - Test fixtures reset before each test.
   !!
   !! Note: Normally test execution order should not matter in a test
   !! session. In this case it *does*, because we are explicitly testing
   !! fixture setup/teardown semantics that depend on the deterministic
   !! alphabetical execution order imposed by `std::map`.

   type(test_session_t) :: test_session
   type(c_ptr) :: test_fixture_ptr, suite_fixture_ptr, session_fixture_ptr
   type(test_fixture_t), target :: test_fixture
   type(suite_fixture_t), target :: suite_fixture
   type(session_fixture_t), target :: session_fixture

   test_fixture_ptr   = c_loc(test_fixture)
   suite_fixture_ptr  = c_loc(suite_fixture)
   session_fixture_ptr = c_loc(session_fixture)

   ! Register session fixture (global across all suites)
   call test_session%register_fixture( &
         setup    = setup_session_fixture, &
         teardown = teardown_session_fixture, &
         args     = session_fixture_ptr, &
         scope    = "session")

   ! Suite A: session-only behavior
   call test_session%register_test_suite( &
         test_suite_name = "a_session_only_suite")

   call test_session%register_test( &
         test_suite_name = "a_session_only_suite", &
         test_name       = "aa_session_fixture_visible", &
         test            = test_session_fixture_visible)

   call test_session%register_test( &
         test_suite_name = "a_session_only_suite", &
         test_name       = "ab_session_fixture_visible", &
         test            = test_session_fixture_visible)

   call test_session%register_test( &
         test_suite_name = "a_session_only_suite", &
         test_name       = "ac_session_fixture_visible", &
         test            = test_session_fixture_visible)

   ! Suite B: suite fixture shared across tests
   call test_session%register_test_suite( &
         test_suite_name = "b_suite_fixture_suite")

   call test_session%register_fixture( &
         setup           = setup_suite_fixture, &
         teardown        = teardown_suite_fixture, &
         args            = suite_fixture_ptr, &
         scope           = "suite", &
         test_suite_name = "b_suite_fixture_suite")

   call test_session%register_test( &
         test_suite_name = "b_suite_fixture_suite", &
         test_name       = "ba_suite_fixture_shared", &
         test            = test_suite_fixture_shared)

   call test_session%register_test( &
         test_suite_name = "b_suite_fixture_suite", &
         test_name       = "bb_suite_fixture_shared", &
         test            = test_suite_fixture_shared)

   call test_session%register_test( &
         test_suite_name = "b_suite_fixture_suite", &
         test_name       = "bc_suite_fixture_shared", &
         test            = test_suite_fixture_shared)

   ! Suite C: suite + test fixture combined behavior
   call test_session%register_test_suite( &
         test_suite_name = "c_suite_and_test_fixture_suite")

   call test_session%register_fixture( &
         setup           = setup_suite_fixture, &
         teardown        = teardown_suite_fixture, &
         args            = suite_fixture_ptr, &
         scope           = "suite", &
         test_suite_name = "c_suite_and_test_fixture_suite")

   call test_session%register_fixture( &
         setup           = setup_test_fixture, &
         teardown        = teardown_test_fixture, &
         args            = test_fixture_ptr, &
         scope           = "test", &
         test_suite_name = "c_suite_and_test_fixture_suite")

   call test_session%register_test( &
         test_suite_name = "c_suite_and_test_fixture_suite", &
         test_name       = "ca_test_fixture_first", &
         test            = test_first_test_fixture_reset)

   call test_session%register_test( &
         test_suite_name = "c_suite_and_test_fixture_suite", &
         test_name       = "cb_test_fixture_second", &
         test            = test_second_test_fixture_reset)

   call test_session%register_test( &
         test_suite_name = "c_suite_and_test_fixture_suite", &
         test_name       = "cc_test_fixture_third", &
         test            = test_third_test_fixture_reset)

   call test_session%run()
   call test_session%finalize()
end program test_fortest_fixtures

