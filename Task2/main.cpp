#include <iostream>
#include <assert.h>
#include <sstream>

#include "matrix.h"
#include "matrix_processor.h"
#include "matrix_io.h"

using namespace std;
using namespace solution;
using namespace solution::matrix_io;

void run_tests() {
    using row_type = vector<int>;
    auto row = row_type{1, 2, 3};

    cout << row << endl;

    // ok
    matrix<int> m2(move(vector<vector<int>>{{1, 2},
                                            {3, 4}}));

    // ok
    matrix<int> m3({{1, 2},
                    {3, 4}});
    assert(m3[1][1] == 4);

    matrix_processor p;

    {
        matrix<int> m_zeros(vector<vector<int>>{{0, 0},
                                                {0, 0}});
        p.process(m_zeros);
        cout << endl << m_zeros << endl;
    }

    // Equality operator
    {
        assert(matrix<int>({{1, 2}}) == matrix<int>({{1, 2}}));
        assert(!(matrix<int>({{1, 2}}) == matrix<int>({{1, 3}})));
    }

    // Input/output
    {
        const char *test_content = "1 2\n3 4\n";
        ostringstream oss;
        oss << m2;
        assert(oss.str() == test_content);

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
}

int main() {
    run_tests();

    return 0;
}
