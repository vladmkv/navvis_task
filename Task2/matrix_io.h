//
// Created by Vladimir on 24.10.2022.
//

#ifndef TASK2_MATRIX_IO_H
#define TASK2_MATRIX_IO_H

#include <iostream>
#include <vector>

#include "matrix.h"

namespace solution {

    namespace matrix_io {

        const char SEPARATOR = ' ';

        template<class T>
        std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
            for (auto item: v) {
                os << item;
            }
            return os;
        }

        template<class T>
        std::ostream &operator<<(std::ostream &os, const matrix<T> &m) {
            for (int row = 0; row < m.getRows(); row++) {
                for (int column = 0; column < m.getColumns(); column++) {
                    os << m[row][column];

                    if (column != m.getColumns() - 1) {
                        os << SEPARATOR;
                    }
                }

                os << std::endl;
            }

            return os;
        }

        template<class T>
        std::istream &operator>>(std::istream &is, matrix<T> &m) {
            m.resize(0, 0);

            int row = 0, column = 0;

            while(!is.eof())
            {
                std::string line;
                std::getline(is, line);
                std::vector<T> current_row;

                // Parse input line into a T vector of unknown size.
                auto itStart = line.cbegin();
                auto itEnd = itStart;
                while (itStart != line.end())
                {
                    itEnd = std::find(itStart, line.cend(), SEPARATOR);
                    std::istringstream iss(std::string(itStart, itEnd));

                    T value;
                    iss >> value;
                    itStart = itEnd;

                    current_row.push_back(value);
                }

                if (m.getRows() == 0)
                {
                    // Initialize matrix first row.
                    m.resize(1, current_row.size());
                }

                if (current_row.size() != m[0].size())
                {
                    throw std::logic_error("Row length inconsistent");
                }

                // Copy valid values to a new row of the matrix.
                m.add_row();
                column = 0;

                for(auto value : current_row)
                {
                    m[row][column++] = value;
                }

                ++row;
            }

            return is;
        }

    };

} // solution

#endif //TASK2_MATRIX_IO_H
