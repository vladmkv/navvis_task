#include <iostream>
#include <cassert>
#include <sstream>

#include "matrix.h"
#include "matrix_processor.h"
#include "matrix_io.h"

using namespace std;
using namespace solution;
using namespace solution::matrix_io;

const char SEPARATOR_SPACE = ' ';
const char SEPARATOR_COMMA = ',';

void run_tests() {
    // Initialization
    {
        matrix<int> m3({{1, 2},
                        {3, 4}});
        assert(m3[1][1] == 4);

    }

    // Equality operator
    {
        assert(matrix<int>({{1, 2}}) == matrix<int>({{1, 2}}));
        assert(!(matrix<int>({{1, 2}}) == matrix<int>({{1, 3}})));
    }

    // Input/output
    {
        matrix<int> m2(std::move(vector<vector<int>>{{1, 2},
                                                     {3, 4}}));
        ostringstream oss;
        oss << m2;
        assert(oss.str() == "1 2\n3 4\n");

        istringstream iss("1");
        matrix<int> m1;
        iss >> m1;
        assert(m1 == matrix<int>({{1}}));

        auto iss2 = istringstream("1 2");
        iss2 >> m1;
        assert(m1 == matrix<int>({{1, 2}}));

        auto iss3 = istringstream("1\n2");
        iss3 >> m1;
        assert(m1 == matrix<int>({{1},
                                  {2}}));
    }

    // Custom separator
    {
        matrix<int> m;
        auto iss = istringstream("1,2");
        parseFromCsvStream(iss, m, SEPARATOR_COMMA);
        assert(m == matrix<int>({{1, 2}}));

        ostringstream oss;
        writeToCsvStream(oss, m, SEPARATOR_COMMA);
        assert(oss.str() == "1,2\n");
    }

    // Float datatype
    {
        matrix<float> m;
        auto iss = istringstream("3.1415 2");
        iss >> m;
        assert(m == matrix<float>({{3.1415, 2}}));

        ostringstream oss;
        oss << m;
        assert(oss.str() == "3.1415 2\n");
    }

    // Double datatype
    {
        matrix<double> m;
        auto iss = istringstream("3.1415,2");
        parseFromCsvStream(iss, m, SEPARATOR_COMMA);
        assert(m == matrix<double>({{3.1415, 2}}));

        ostringstream oss;
        writeToCsvStream(oss, m, SEPARATOR_COMMA);
        assert(oss.str() == "3.1415,2\n");
    }

    // Processing
    matrix_processor p;
    {
        matrix<float> m({{0, 1},
                         {1, 1}});
        p.process(m);

        // Expecting average of two adjacent cells (0 + 1) / 2 = 0.5
        assert(m[0][0] == 0.5);
    }

    // File I/O -- ints + spaces
    {
        auto m = load<int>("input1.csv", SEPARATOR_SPACE);
        assert(m[1][1] == 0);

        p.process(m);
        assert(m[1][1] == 5);

        auto output_file = "output1.csv";
        save(output_file, m, SEPARATOR_SPACE);
        ifstream f(output_file);
        assert(f.good());
    }

    // File I/O -- floats + commas
    {
        auto m = load<float>("input2.csv", SEPARATOR_COMMA);
        assert(m[1][1] == 0);

        p.process(m);
        assert(m[1][1] == 1.5);

        auto output_file = "output2.csv";
        save<float>(output_file, m, SEPARATOR_COMMA);
        ifstream f(output_file);
        assert(f.good());
    }

    cout << "Tests passed" << endl;
}

static void show_usage(std::string name) {
    string slashes = "/\\";
    auto itLastSlash = std::find_first_of(name.rbegin(), name.rend(), slashes.begin(), slashes.end());

    if (itLastSlash != name.rend()) {
        name = name.substr(name.rend() - itLastSlash);
    }

    cout << "Usage: " << name << " <input file> <output file> --float|--int --space|--comma" << endl <<
         "Example: " << name << " input1.csv output.csv --int --space" << endl;
}

template<class T>
void run_processing(const string &input_file, const string &output_file, const char separator) {
    auto m = load<T>(input_file, separator);

    matrix_processor p;
    p.process(m);

    save(output_file, m, separator);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        // Simple way to run unit tests
        run_tests();

        show_usage(argv[0]);
        return 0;
    }

    vector<string> arguments;
    std::transform(argv + 1, argv + argc, back_inserter(arguments),
                   [](const char *pstr) {
                       return string(pstr);
                   });

    if (arguments.size() < 3) {
        cout << "Not enough arguments" << endl;
        return 0;
    }

    auto input_file = arguments[0];
    auto output_file = arguments[1];
    auto data_type = arguments[2];
    char separator = SEPARATOR_SPACE;

    if (arguments.size() > 3 && arguments[3] == "--comma") {
        separator = SEPARATOR_COMMA;
    }

    try {
        if (data_type == "--float") {
            run_processing<float>(input_file, output_file, separator);
        }
        if (data_type == "--int") {
            run_processing<int>(input_file, output_file, separator);
        } else {
            cout << "Unknown data type!" << endl;
        }
    }
    catch (exception &e) {
        cout << "Error occurred. " << e.what() << endl;
    }

    return 0;
}
