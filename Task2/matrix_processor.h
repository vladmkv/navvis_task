//
// Created by Vladimir on 23.10.2022.
//

#ifndef TASK2_MATRIX_PROCESSOR_H
#define TASK2_MATRIX_PROCESSOR_H

#include "matrix.h"

namespace solution {

    class matrix_processor {
    private:
        // Interpolation value for out-of-boundary cells
        const int INTERPOLATE_DEFAULT_VALUE = 0;

        bool isZero(int x) {
            return x == 0;
        }

        template<class T>
        T get(const matrix<T> &m, int row, int column) {
            if (row < 0 || row >= m.getRows() || column < 0 || column >= m.getColumns()) {
                return static_cast<T>(INTERPOLATE_DEFAULT_VALUE);
            }

            return m[row][column];
        }

        template<class T>
        T interpolate(const matrix<T> &m, int row, int column) {
            T sum = get(m, row - 1, column) +
                    get(m, row + 1, column) +
                    get(m, row, column - 1) +
                    get(m, row, column + 1);
            return static_cast<T>(sum / 4.0);
        }

        template<class T>
        void visitor(matrix<T> &m, int row, int column) {
            if (isZero(m[row][column])) {
                m[row][column] = interpolate(m, row, column);
            }
        }

    public:
        // Sample processing function. Averages zero cell among sibling cells.
        template<class T>
        void process(matrix<T> &m) {
            for (int row = 0; row < m.getRows(); ++row) {
                for (int column = 0; column < m.getColumns(); ++column) {
                    visitor(m, row, column);
                }
            }
        }
    };

} // solution

#endif //TASK2_MATRIX_PROCESSOR_H
