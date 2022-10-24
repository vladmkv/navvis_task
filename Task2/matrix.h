//
// Created by Vladimir on 23.10.2022.
//

#ifndef TASK2_MATRIX_H
#define TASK2_MATRIX_H

#include <vector>

namespace solution {

    template<class T>
    class matrix {
    private:
        using row_type = std::vector<T>;

        int _rows = 0;
        int _columns = 0;

        std::vector<row_type> _elements;

    public:
        matrix(const matrix &) = delete;

        matrix() = default;

        matrix(std::vector<row_type> &&rows) {
            std::swap(_elements, rows);
            _rows = _elements.size();
            _columns = _rows > 0 ? _elements[0].size() : 0;
        }

        int getRows() const {
            return _rows;
        }

        int getColumns() const {
            return _columns;
        }

        void resize(int rows, int columns) {
            _columns = columns;
            _rows = 0;
            _elements.clear();

            for (int i = 0; i < rows; ++i) {
                addRow();
            }
        }

        void addRow() {
            _elements.push_back(std::vector<T>(_columns));
            ++_rows;
        }

        std::vector<T> &operator[](int row) {
            if (row < 0 || row >= _rows) {
                throw std::out_of_range("Index out of range!");
            }

            return _elements[row];
        }

        const std::vector<T> &operator[](int row) const {
            if (row < 0 || row >= _rows) {
                throw std::out_of_range("Index out of range!");
            }

            return _elements[row];
        }

        bool operator==(const matrix<T> &other) const {
            return _elements == other._elements;
        }
    };

} // solution

#endif //TASK2_MATRIX_H
