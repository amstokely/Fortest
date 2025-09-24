# Fortest – A Fortran Test Framework
# Fortest

[![codecov](https://codecov.io/gh/andystokely/fortest/branch/main/graph/badge.svg)](https://codecov.io/gh/andystokely/fortest)

**Fortest** is a lightweight unit testing framework designed specifically for Fortran.
Its purpose is to make writing tests straightforward, lowering the barrier for developers who want to improve the reliability of their code.

All tests are written in **pure, standard Fortran** with no external libraries, complex OOP features, or compiler-specific extensions required.

---

## Features

* **Pure Fortran tests**
  Write tests in plain Fortran. Test code can double as example code and works seamlessly with IDEs and tooling.

* **Zero external dependencies**
  No need to install or link against third-party libraries.

* **Simple, familiar syntax**
  Assertions cover common checks (equality, inequality, logical conditions, string comparisons, exceptions) without requiring advanced Fortran OOP features.

* **Fixture support**
  Reuse setup and teardown logic with flexible scopes:

  * **test**: run before and after each test, ideal for isolating repetitive setup and teardown code
  * **suite**: run once per suite, useful for initializing resources shared by related tests
  * **session**: run once for the entire session, well suited for MPI-based tests or tests that depend on global state

* **Fixtures are optional**
  Simple tests do not need fixtures.

* **Structured logging**
  Automatic logging of sessions, suites, tests, and assertions with clear, colorized pass and fail output.

* **Easy build system integration**
  Works out of the box with **CMake** and **GNU Make**.

---

## Installation

Fortest uses CMake as its build system. To install locally:

```bash
# From the root of the Fortest repository
mkdir build
cd build

# Configure the project
cmake <FORTEST_ROOT_DIR> -DCMAKE_INSTALL_PREFIX=<install_directory>

# Build and install
make
make install

# (Optional) Run the test suite
ctest
```

After installation, the CMake configuration files are located in:

```
<install_directory>/lib/cmake/Fortest/
```

When using Fortest in another CMake project, you must set the `Fortest_DIR` variable to this path so that `find_package(Fortest)` can locate it:

```cmake
set(Fortest_DIR "<install_directory>/lib/cmake/Fortest/")
find_package(Fortest REQUIRED)
```

You can then link against Fortest in your project with:

```cmake
target_link_libraries(mytests PRIVATE Fortest::fortest)
```

### The `fortest-config` Utility

Fortest provides a helper executable, **`fortest-config`**, which makes it easier to integrate with external build systems.
This binary is built into both:

* `<build_dir>/bin` (before installation)
* `<install_dir>/bin` (after installation)

By default, Fortest installs into `<build_dir>/install`.

The `fortest-config` utility can be used to query installation details:

* `--libdir` → Library directory
* `--moddir` → Fortran module directory
* `--libs`   → Linking flags required for the compiler
* `--includedir` → Include directory
* `--prefix` → Full install prefix directory
* `--cmake-prefix` → Path to the CMake package configuration

This makes it easy to integrate Fortest into Makefiles, custom scripts, or Fortran build systems without hardcoding paths.

---

## Tutorial Example: Math Operations

The `examples/` directory contains a tutorial example that demonstrates how to write tests using Fortest.

This example defines and tests a small set of **math operations**:

* **Matrix multiplication**
* **Cross product**
* **Vector normalization**

It also illustrates how to use both **suite-level fixtures** (for reusable data like matrices and vectors) and **test-level fixtures** (for scratch data that is reset before each test).

### Running the Example

From your build directory, run:

```bash
./examples/test_math_ops
```

You should see output like:

```
[INFO] Starting test session: 
[INFO] Running test suite: matrix_mul_suite
[INFO] Running test: test_cross_product_anticommutative
[PASS] Test passed: test_cross_product_anticommutative
[INFO] Running test: test_cross_product_expected
[PASS] Test passed: test_cross_product_expected
...
[INFO] Finished test session: 
```

This example is a good starting point for learning Fortest, since it shows how to:

1. Write test routines with the required `(t_ptr, ts_ptr, s_ptr)` signature
2. Register a test suite
3. Register both suite-level and test-level fixtures
4. Register individual tests
5. Run all tests and finalize the session
