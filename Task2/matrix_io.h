//
// Created by Vladimir on 24.10.2022.
//

#ifndef TASK2_MATRIX_IO_H
#define TASK2_MATRIX_IO_H

#include <iostream>
#include <fstream>
#include <vector>

#include "matrix.h"

namespace solution {

    namespace matrix_io {
        const char DEFAULT_SEPARATOR = ' ';

        template<class T>
        std::ostream &
        writeToCsvStream(std::ostream &os, const matrix<T> &m, const char separator = DEFAULT_SEPARATOR) {
            for (int row = 0; row < m.getRows(); row++) {
                for (int column = 0; column < m.getColumns(); column++) {
                    os << m[row][column];

                    if (column != m.getColumns() - 1) {
                        os << separator;
                    }
                }

                os << std::endl;
            }

            return os;
        }

        template<class T>
        std::istream &parseFromCsvStream(std::istream &is, matrix<T> &m, const char separator = DEFAULT_SEPARATOR) {
            m.resize(0, 0);
            int row = 0, column = 0;

            while (!is.eof()) {
                std::string line;
                std::getline(is, line);

                if (!line.length()) {
                    throw std::logic_error("Empty line");
                }

                std::vector<std::string> line_items;
                std::istringstream iss_line(line);
                while (!iss_line.eof()) {
                    std::string item;
                    std::getline(iss_line, item, separator);
                    line_items.push_back(item);
                }

                // Parse individual strings to T
                std::vector<T> current_row;
                std::transform(line_items.begin(), line_items.end(), std::back_inserter(current_row),
                               [](const std::string &s) {
                                   T value;
                                   std::istringstream iss(s);
                                   iss >> value;
                                   return value;
                               });

                if (m.getRows() == 0) {
                    // Initialize matrix first row.
                    m.resize(1, current_row.size());
                } else {
                    m.addRow();
                }

                if (current_row.size() != m.getColumns()) {
                    throw std::logic_error("Columns amount inconsistent");
                }

                // Copy valid values to a new row of the matrix.
                column = 0;

                for (auto value: current_row) {
                    m[row][column++] = value;
                }

                ++row;
            }

            return is;
        }

        // Convenience operators overload for matrix I/O
        template<class T>
        std::ostream &operator<<(std::ostream &os, const matrix<T> &m) {
            return writeToCsvStream<T>(os, m, DEFAULT_SEPARATOR);
        }

        template<class T>
        std::istream &operator>>(std::istream &is, matrix<T> &m) {
            return parseFromCsvStream(is, m);
        }

        template<class T, char sep = ' '>
        matrix<T> load(const std::string &file_name) {
            std::ifstream file(file_name);
            matrix<T> m;
            parseFromCsvStream(file, m, sep);
            return m;
        }

        template<class T, char sep = ' '>
        void save(const std::string &file_name, const matrix<T> &m) {
            std::ofstream file(file_name);
            writeToCsvStream(file, m, sep);
        }
    };

} // solution

#endif //TASK2_MATRIX_IO_H
