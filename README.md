# Task 1
Folder `Task1-CodeAnalysis` contains files with code review. My comments start with `"// VM:"`

# Task 2
Folder `Task2` contains source for CSV File task.

Project can be built using CMake (tested on MacOS):

`cmake .`

`cmake --build .`

Executable is called Task2. There is a couple of sample `input*.csv` files.

`Usage: Task2 <input file> <output file> --float|--int --space|--comma
Example: Task2 input1.csv output.csv --int --space`

## Implemenation notes
Header-only library. To install, copy header files to your project.

Classes are in `solution` namespace to prevent global namespace pollution.

### Main classes
`matrix<T>` represents a 2D matrix of type `T`. It supports built-in numeric types, for example, `int` and `double`.

`matrix_processor` encapsulates processing of matrix. I implemented basic 'interpolation' by averaging neighbor cells. It can be customized according to needs, see `visitor()` method.

`matrix_io` namespace contains implementation and convenience operators for reading and writing `matrix` to and from STL streams.

`main.cpp` contains argument parsing and call of actual processing of CSV file. It also contains `run_tests()` function.

If error occurs, and `std::exception` is thrown.

### Tests
I implemented a number of unit tests for my classes and functions along the develpment. They all are grouped in the `run_tests()` function. It is run when application is executed without arguments. If a test fails, `assert()` function prints an error message in debug build.

In real world scenario I would propose moving tests to separate files and running them using some runner like Google Test.
