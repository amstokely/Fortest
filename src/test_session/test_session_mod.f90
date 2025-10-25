!> @page fortest_test_session Test Session Management
!>
!> @brief Manage test sessions, suites, and fixtures in Fortest.
!>
!> A test session is the top-level container for organizing and executing tests.
!> Each session can hold multiple test suites, and each suite can hold multiple tests.
!> Fixtures can be registered at the **test**, **suite**, or **session** level.
!>
!> See the Fortran tests in this repository for usage examples.
module fortest_test_session

    use fortest_test_suite, only : test_suite_t
    use procedure_interfaces_mod, only : test_proc, fixture_proc, param_test_proc
    use iso_c_binding
    implicit none

    public :: test_session_t

    interface register_parameterized_test
        module procedure register_parameterized_test_with_num_params
        module procedure register_parameterized_test_with_indices
    end interface register_parameterized_test

    !> @brief Linked list node for holding a test suite.
    type :: test_suite_node_t
        type(test_suite_t) :: suite   !! The test suite data
        type(test_suite_node_t), pointer :: next => null() !! Pointer to next node
    end type test_suite_node_t

    !> @brief A test session containing suites and global controls.
    type :: test_session_t
        private
        type(test_suite_node_t), pointer :: head => null() !! Head of suite list
        type(test_suite_node_t), pointer :: tail => null() !! Tail of suite list

    contains
        procedure :: register_test_suite  !! Add a new test suite
        procedure :: get_test_suite       !! Find a suite by name
        procedure :: register_fixture     !! Register a fixture with a suite
        procedure :: register_test        !! Register a test in a suite
        procedure :: register_parameterized_test_with_num_params
        procedure :: register_parameterized_test_with_indices
        generic :: register_parameterized_test => &
                register_parameterized_test_with_num_params, &
                register_parameterized_test_with_indices
        procedure :: run                  !! Run all registered tests
        procedure, public :: finalize     !! Finalize session and exit with status
        procedure, public :: get_status   !! Aggregate test status across suites
    end type test_session_t

contains

    !> @brief Register a new test suite and add it to the session.
    !> @param this The test session
    !> @param test_suite_name Name of the new test suite
    subroutine register_test_suite(this, test_suite_name)
        use fortest_test_suite, only : test_suite_t
        use f_c_string_t_mod, only : f_c_string_t

        class(test_session_t), intent(inout) :: this
        character(len = *), intent(in) :: test_suite_name
        type(test_suite_t) :: test_suite
        type(f_c_string_t) :: f_c_string_test_suite_name
        type(test_suite_node_t), pointer :: new_node
        integer :: status

        interface
            subroutine c_register_test_suite(name) bind(C, name = "c_register_test_suite")
                import :: c_ptr
                type(c_ptr), value :: name
            end subroutine c_register_test_suite
        end interface

        ! Initialize and register suite
        test_suite%name = test_suite_name
        f_c_string_test_suite_name = f_c_string_t(test_suite_name)
        status = f_c_string_test_suite_name%to_c()
        call c_register_test_suite(f_c_string_test_suite_name%get_c_string())

        ! Add to linked list
        allocate(new_node)
        new_node%suite = test_suite
        new_node%next => null()

        if (.not. associated(this%head)) then
            this%head => new_node
            this%tail => new_node
        else
            this%tail%next => new_node
            this%tail => new_node
        end if
    end subroutine register_test_suite

    !> @brief Register a test with a suite.
    !> @param this The test session
    !> @param test_suite_name Name of the suite
    !> @param test_name Name of the test
    !> @param test Test procedure to register
    subroutine register_test(this, test_suite_name, test_name, test)
        use f_c_string_t_mod, only : f_c_string_t
        class(test_session_t), intent(in) :: this
        character(len = *), intent(in) :: test_suite_name
        character(len = *), intent(in) :: test_name
        procedure(test_proc) :: test
        type(f_c_string_t) :: f_c_string_test_suite_name
        type(f_c_string_t) :: f_c_string_test_name
        integer :: status

        interface
            subroutine c_register_test(test_suite_name, test_name, test) bind(C, name = "c_register_test")
                import :: c_ptr, c_funptr
                type(c_ptr), value :: test_suite_name
                type(c_ptr), value :: test_name
                type(c_funptr), value :: test
            end subroutine c_register_test
        end interface

        f_c_string_test_suite_name = f_c_string_t(test_suite_name)
        status = f_c_string_test_suite_name%to_c()
        f_c_string_test_name = f_c_string_t(test_name)
        status = f_c_string_test_name%to_c()
        call c_register_test(&
                f_c_string_test_suite_name%get_c_string(), &
                f_c_string_test_name%get_c_string(), &
                c_funloc(test))
    end subroutine register_test

    subroutine register_parameterized_test_with_num_params(this, test_suite_name, test_name, test, num_params)
        use f_c_string_t_mod, only : f_c_string_t
        class(test_session_t), intent(in) :: this
        character(len = *), intent(in) :: test_suite_name
        character(len = *), intent(in) :: test_name
        procedure(param_test_proc) :: test
        integer(c_int), intent(in) :: num_params
        integer(c_int), dimension(:), allocatable :: params
        integer :: i

        allocate(params(num_params))
        params = [(i, i = 1, num_params)]
        call this%register_parameterized_test_with_indices(test_suite_name, test_name, test, params)
    end subroutine register_parameterized_test_with_num_params

    !> @brief Register a parameterized test with a suite.
    !>
    !> @param this The test session
    !> @param test_suite_name Name of the suite
    !> @param test_name Name of the test
    !> @param test Test procedure to register
    !> @param params Array of parameter indices
    subroutine register_parameterized_test_with_indices(this, test_suite_name, test_name, test, params)
        use f_c_string_t_mod, only : f_c_string_t
        class(test_session_t), intent(in) :: this
        character(len = *), intent(in) :: test_suite_name
        character(len = *), intent(in) :: test_name
        procedure(param_test_proc) :: test
        integer(c_int), dimension(:), target, intent(in) :: params

        type(f_c_string_t) :: f_c_suite, f_c_name
        integer :: status

        interface
            subroutine c_register_parameterized_test(&
                    suite_name, test_name, test, params, nparams) bind(C, name = "c_register_parameterized_test")
                import :: c_ptr, c_funptr, c_int
                type(c_ptr), value :: suite_name
                type(c_ptr), value :: test_name
                type(c_funptr), value :: test
                type(c_ptr), value :: params
                integer(c_int), value :: nparams
            end subroutine c_register_parameterized_test
        end interface

        ! Convert strings
        f_c_suite = f_c_string_t(test_suite_name)
        status = f_c_suite%to_c()
        f_c_name = f_c_string_t(test_name)
        status = f_c_name%to_c()

        ! Call C wrapper
        call c_register_parameterized_test(&
                f_c_suite%get_c_string(), &
                f_c_name%get_c_string(), &
                c_funloc(test), &
                c_loc(params(1)), &
                size(params, kind = c_int))
    end subroutine register_parameterized_test_with_indices

    !> @brief Register a fixture with setup/teardown for this suite.
    !!
    !! @param[in] this    The test suite.
    !! @param[in] setup   Setup procedure pointer.
    !! @param[in] teardown Teardown procedure pointer.
    !! @param[in] args    Opaque pointer to fixture arguments.
    !! @param[in] scope   Scope string: "test", "suite", or "session".
    !! @param[in] test_suite_name Name of the suite to attach the fixture to.
    subroutine register_fixture(this, setup, teardown, args, scope, test_suite_name)
        use iso_c_binding
        use f_c_string_t_mod, only : f_c_string_t
        class(test_session_t), intent(in) :: this
        procedure(fixture_proc) :: setup
        procedure(fixture_proc) :: teardown
        type(c_ptr), value :: args
        character(len = *), intent(in) :: scope
        character(len = *), intent(in), optional :: test_suite_name
        type(f_c_string_t) :: f_c_string_test_suite_name
        type(f_c_string_t) :: f_c_string_scope
        integer :: status
        interface
            subroutine c_register_fixture(test_suite_name, &
                    setup, teardown, args, scope) bind(C, name = "c_register_fixture")
                import :: c_ptr, c_funptr
                type(c_ptr), value :: test_suite_name
                type(c_funptr), value :: setup
                type(c_funptr), value :: teardown
                type(c_ptr), value :: args
                type(c_ptr), value :: scope
            end subroutine c_register_fixture
        end interface

        if (present(test_suite_name)) then
            f_c_string_test_suite_name = f_c_string_t(test_suite_name)
        else
            f_c_string_test_suite_name = f_c_string_t("")
        end if
        f_c_string_test_suite_name = f_c_string_t(test_suite_name)
        status = f_c_string_test_suite_name%to_c()
        f_c_string_scope = f_c_string_t(scope)
        status = f_c_string_scope%to_c()
        call c_register_fixture(&
                f_c_string_test_suite_name%get_c_string(), &
                c_funloc(setup), &
                c_funloc(teardown), &
                args, &
                f_c_string_scope%get_c_string())
    end subroutine register_fixture

    !> @brief Run all registered test suites in this session.
    subroutine run(this)
        class(test_session_t), intent(in) :: this
        interface
            subroutine c_run_test_session() bind(C, name = "c_run_test_session")
            end subroutine c_run_test_session
        end interface
        call c_run_test_session()
    end subroutine run

    !> @brief Get aggregated status from all test suites.
    !> @param this The test session
    !> @return Sum of suite statuses (0 if all passed)
    function get_status(this) result(status)
        class(test_session_t), intent(inout) :: this
        type(test_suite_node_t), pointer :: node
        integer :: status

        status = 0
        node => this%head
        do while (associated(node))
            status = status + node%suite%get_status()
            node => node%next
        end do
    end function get_status

    !> @brief Finalize the test session and exit with status.
    !> @param this The test session
    subroutine finalize(this)
        class(test_session_t), intent(inout) :: this
        type(test_suite_node_t), pointer :: node, temp
        integer :: status

        status = this%get_status()
        node => this%head
        do while (associated(node))
            temp => node%next
            deallocate(node)
            node => temp
        end do
        this%head => null()
        this%tail => null()
        call exit(status)
    end subroutine finalize

    !> @brief Find a test suite by name.
    !> @param this The test session
    !> @param name Suite name
    !> @return Pointer to the test suite, or null if not found
    function get_test_suite(this, name) result(suite_ptr)
        class(test_session_t), intent(in) :: this
        character(len = *), intent(in) :: name
        type(test_suite_t), pointer :: suite_ptr
        type(test_suite_node_t), pointer :: node

        suite_ptr => null()
        node => this%head
        do while (associated(node))
            if (trim(node%suite%name) == trim(name)) then
                suite_ptr => node%suite
                return
            end if
            node => node%next
        end do
    end function get_test_suite

end module fortest_test_session
