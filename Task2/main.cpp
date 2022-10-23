#include <iostream>
#include <assert.h>

#include "matrix.h"
#include "matrix_processor.h"

using namespace std;
using namespace solution;

void run_tests() {
    matrix<int> m(3, 3);
    using row_type = vector<int>;
    auto row = row_type{1, 2, 3};

    // ok
    matrix<int> m2(move(vector<vector<int>>{{1, 2},
                                            {3, 4}}));

    // ok
    matrix<int> m3({{1, 2},
                    {3, 4}});
    assert(m3[1][1] == 4);

    m[1][0] = 2;

    cout << endl << m[1][0] << endl;

    matrix_processor p;

    {
        matrix<int> m_zeros(vector<vector<int>>{{0, 0},
                                                {0, 0}});
        p.process(m_zeros);
        cout << endl << m_zeros[0][0] << endl;
    }
}

int main() {
    run_tests();

    return 0;
}
