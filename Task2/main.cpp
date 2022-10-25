#include <iostream>
#include <cassert>
#include <sstream>

#include "matrix.h"
#include "matrix_processor.h"
#include "matrix_io.h"

using namespace std;
using namespace solution;
using namespace solution::matrix_io;

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
        parseFromCsvStream(iss, m, ',');
        assert(m == matrix<int>({{1, 2}}));

        ostringstream oss;
        writeToCsvStream(oss, m, ',');
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
        parseFromCsvStream(iss, m, ',');
        assert(m == matrix<double>({{3.1415, 2}}));

        ostringstream oss;
        writeToCsvStream(oss, m, ',');
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
        auto m = load<int, ' '>("input1.csv");
        assert(m[1][1] == 0);

        p.process(m);
        assert(m[1][1] == 5);

        auto output_file = "output1.csv";
        save(output_file, m);
        ifstream f(output_file);
        assert(f.good());
    }

    // File I/O -- floats + commas
    {
        auto m = load<float, ','>("input2.csv");
        assert(m[1][1] == 0);

        p.process(m);
        assert(m[1][1] == 1.5);

        auto output_file = "output2.csv";
        save<float, ','>(output_file, m);
        ifstream f(output_file);
        assert(f.good());
    }

    cout << "Tests passed" << endl;
}

int main() {
    run_tests();

    return 0;
}
