#include <iostream>

#include "matrix.h"

using namespace std;
using namespace solution;

int main() {
    matrix<int> m(3, 3);

    m[1][0] = 2;

    cout << endl << m[1][0] << endl;
    return 0;
}
