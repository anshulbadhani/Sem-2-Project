#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <cassert>

inline constexpr double PI = 3.14159265358979323846;

// Template class for MxN matrix
template <typename T>
class Matrix {
protected:
    std::size_t rows, cols; // Matrix dimensions MxN
    T** mat;                // Dynamic allocation

public:
    Matrix(std::size_t m, std::size_t n) : rows(m), cols(n) {
        mat = new T*[rows];
        for (std::size_t i = 0; i < rows; ++i) {
            mat[i] = new T[cols]{0}; // Initialize with zeros
        }
    }

    virtual ~Matrix() {
        for (std::size_t i = 0; i < rows; ++i) {
            delete[] mat[i];
        }
        delete[] mat;
    }

    void inputMatrix() {
        std::cout << "Enter elements of " << rows << "x" << cols << " matrix (row-wise):\n";
        for (std::size_t i = 0; i < rows; ++i) {
            for (std::size_t j = 0; j < cols; ++j) {
                std::cin >> mat[i][j];
            }
        }
    }

    virtual void transform() {}

    void display() const {
        for (std::size_t i = 0; i < rows; ++i) {
            for (std::size_t j = 0; j < cols; ++j) {
                std::cout << mat[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    T getElement(std::size_t i, std::size_t j) const {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Matrix indices out of range");
        }
        return mat[i][j];
    }

    void setElement(std::size_t i, std::size_t j, T value) {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Matrix indices out of range");
        }
        mat[i][j] = value;
    }

    std::size_t getRows() const { return rows; }
    std::size_t getCols() const { return cols; }
};

// Derived class for Shearing (valid for at least 2x2 matrices)
template <typename T>
class ShearMatrix : public Matrix<T> {
public:
    ShearMatrix(std::size_t m, std::size_t n, T shearX, T shearY) : Matrix<T>(m, n) {
        if (m < 2 || n < 2) {
            throw std::invalid_argument("ShearMatrix requires at least 2x2 dimensions");
        }
        this->mat[0][1] = shearX;
        this->mat[1][0] = shearY;
    }

    void transform() override {
        std::cout << "Shearing Matrix:" << std::endl;
        this->display();
    }
};

// Derived class for Rotation (only valid for 2x2 matrices)
template <typename T>
class RotateMatrix : public Matrix<T> {
public:
    RotateMatrix(T angle) : Matrix<T>(2, 2) {
        T radians = (PI / 180) * angle;
        this->mat[0][0] = std::cos(radians);
        this->mat[0][1] = -std::sin(radians);
        this->mat[1][0] = std::sin(radians);
        this->mat[1][1] = std::cos(radians);
    }

    void transform() override {
        std::cout << "Rotation Matrix (2x2 only):" << std::endl;
        this->display();
    }
};

// Derived class for Addition (same-sized matrices MxN)
template <typename T>
class AddMatrix : public Matrix<T> {
public:
    AddMatrix(const Matrix<T>& m1, const Matrix<T>& m2) : Matrix<T>(m1.getRows(), m1.getCols()) {
        if (m1.getRows() != m2.getRows() || m1.getCols() != m2.getCols()) {
            throw std::invalid_argument("Matrix sizes do not match for addition");
        }
        for (std::size_t i = 0; i < this->getRows(); ++i) {
            for (std::size_t j = 0; j < this->getCols(); ++j) {
                this->mat[i][j] = m1.getElement(i, j) + m2.getElement(i, j);
            }
        }
    }

    void transform() override {
        std::cout << "Addition Result:" << std::endl;
        this->display();
    }
};

// Derived class for Subtraction (same-sized matrices MxN)
template <typename T>
class SubtractMatrix : public Matrix<T> {
public:
    SubtractMatrix(const Matrix<T>& m1, const Matrix<T>& m2) : Matrix<T>(m1.getRows(), m1.getCols()) {
        if (m1.getRows() != m2.getRows() || m1.getCols() != m2.getCols()) {
            throw std::invalid_argument("Matrix sizes do not match for subtraction");
        }
        for (std::size_t i = 0; i < this->getRows(); ++i) {
            for (std::size_t j = 0; j < this->getCols(); ++j) {
                this->mat[i][j] = m1.getElement(i, j) - m2.getElement(i, j);
            }
        }
    }

    void transform() override {
        std::cout << "Subtraction Result:" << std::endl;
        this->display();
    }
};

#endif