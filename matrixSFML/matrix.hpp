#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace std;

const double PI = 3.14159265358979323846;

// Template base class
template <typename T>
class Matrix {
protected:
    size_t rows, cols;
    T** mat;

public:
    // Default constructor - properly initialize mat to nullptr
    Matrix() : rows(0), cols(0), mat(nullptr) {}
   
    // Parameterized constructor
    Matrix(size_t m, size_t n);
   
    // Copy constructor (needed to prevent shallow copying)
    Matrix(const Matrix<T>& other);
   
    // Assignment operator
    Matrix<T>& operator=(const Matrix<T>& other);
   
    virtual ~Matrix();

    void inputMatrix();
    void display() const;

    T getElement(size_t i, size_t j) const;
    void setElement(size_t i, size_t j, T value);

    size_t getRows() const;
    size_t getCols() const;

    Matrix<T> operator+(const Matrix<T>& other) const;
    Matrix<T> operator-(const Matrix<T>& other) const;
    Matrix<T> operator*(const Matrix<T>& other) const;
   
    // Vector projection implementation
    static Matrix<T> projection(const Matrix<T>& vec1, const Matrix<T>& vec2);
};

// Shearing
template <typename T>
class ShearMatrix : public Matrix<T> {
public:
    // Default constructor
    ShearMatrix() : Matrix<T>() {}
   
    ShearMatrix(size_t m, size_t n, T shearX, T shearY);
    void transform();
};

// Rotation
template <typename T>
class RotateMatrix : public Matrix<T> {
public:
    // Default constructor
    RotateMatrix() : Matrix<T>() {}
   
    RotateMatrix(T angle);
    void transform();
};

// Scaling
template <typename T>
class ScaleMatrix : public Matrix<T> {
public:
    // Default constructor
    ScaleMatrix() : Matrix<T>() {}
   
    ScaleMatrix(size_t m, size_t n, T scaleX, T scaleY);
    void transform();
};

// Reflection
template <typename T>
class ReflectMatrix : public Matrix<T> {
public:
    // Default constructor
    ReflectMatrix() : Matrix<T>() {}
   
    ReflectMatrix(size_t m, size_t n, bool reflectX, bool reflectY);
    void transform();
};



// ------------------ DEFINITIONS ------------------

template <typename T>
Matrix<T>::Matrix(size_t m, size_t n) : rows(m), cols(n) {
    mat = new T*[rows];
    for (size_t i = 0; i < rows; i++) {
        mat[i] = new T[cols]{0};
    }
}

// Copy constructor implementation
template <typename T>
Matrix<T>::Matrix(const Matrix<T>& other) : rows(other.rows), cols(other.cols) {
    // Allocate memory
    mat = nullptr;
    if (rows > 0 && cols > 0) {
        mat = new T*[rows];
        for (size_t i = 0; i < rows; i++) {
            mat[i] = new T[cols];
            // Copy values
            for (size_t j = 0; j < cols; j++) {
                mat[i][j] = other.mat[i][j];
            }
        }
    }
}

// Assignment operator implementation
template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) {
    if (this != &other) {
        // Clean up existing resources
        if (mat != nullptr) {
            for (size_t i = 0; i < rows; i++) {
                delete[] mat[i];
            }
            delete[] mat;
        }
       
        // Copy from other
        rows = other.rows;
        cols = other.cols;
       
        mat = nullptr;
        if (rows > 0 && cols > 0) {
            mat = new T*[rows];
            for (size_t i = 0; i < rows; i++) {
                mat[i] = new T[cols];
                for (size_t j = 0; j < cols; j++) {
                    mat[i][j] = other.mat[i][j];
                }
            }
        }
    }
    return *this;
}

template <typename T>
Matrix<T>::~Matrix() {
    if (mat != nullptr) {
        for (size_t i = 0; i < rows; i++) {
            delete[] mat[i];
        }
        delete[] mat;
    }
}

template <typename T>
void Matrix<T>::inputMatrix() {
    if (rows == 0 || cols == 0) {
        throw runtime_error("Cannot input elements into a matrix with zero dimensions");
    }
   
    cout << "Enter elements of " << rows << "x" << cols << " matrix (row-wise):\n";
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            cin >> mat[i][j];
        }
    }
}

template <typename T>
void Matrix<T>::display() const {
    if (mat == nullptr) {
        cout << "Empty matrix" << endl;
        return;
    }
   
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

template <typename T>
T Matrix<T>::getElement(size_t i, size_t j) const {
    if (i >= rows || j >= cols || mat == nullptr) {
        throw std::out_of_range("Matrix index out of range");
    }
    return mat[i][j];
}

template <typename T>
void Matrix<T>::setElement(size_t i, size_t j, T value) {
    if (i >= rows || j >= cols || mat == nullptr) {
        throw std::out_of_range("Matrix index out of range");
    }
    mat[i][j] = value;
}

template <typename T>
size_t Matrix<T>::getRows() const {
    return rows;
}

template <typename T>
size_t Matrix<T>::getCols() const {
    return cols;
}

template <typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw runtime_error("Error: Matrix sizes do not match for addition!");
    }
    Matrix<T> result(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result.mat[i][j] = mat[i][j] + other.mat[i][j];
        }
    }
    return result;
}

template <typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw runtime_error("Error: Matrix sizes do not match for subtraction!");
    }
    Matrix<T> result(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result.mat[i][j] = mat[i][j] - other.mat[i][j];
        }
    }
    return result;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& other) const {
    if (cols != other.rows) {
        throw runtime_error("Error: Matrix sizes do not match for multiplication!");
    }
    Matrix<T> result(rows, other.cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < other.cols; j++) {
            for (size_t k = 0; k < cols; k++) {
                result.mat[i][j] += mat[i][k] * other.mat[k][j];
            }
        }
    }
    return result;
}

// Vector projection implementation
template <typename T>
Matrix<T> Matrix<T>::projection(const Matrix<T>& vec1, const Matrix<T>& vec2) {
    // Check if both are column vectors
    if (vec1.cols != 1 || vec2.cols != 1 || vec1.rows < 2 || vec2.rows < 2) {
        throw runtime_error("Error: Both matrices must be column vectors for projection!");
    }
   
    // Calculate dot product of vec2 and vec1
    T dotProduct = 0;
    for (size_t i = 0; i < vec1.rows; i++) {
        dotProduct += vec1.mat[i][0] * vec2.mat[i][0];
    }
   
    // Calculate magnitude squared of vec1
    T magnitudeSquared = 0;
    for (size_t i = 0; i < vec1.rows; i++) {
        magnitudeSquared += vec1.mat[i][0] * vec1.mat[i][0];
    }
   
    if (magnitudeSquared == 0) {
        throw runtime_error("Error: Cannot project onto a zero vector!");
    }
   
    // Calculate the scalar projection
    T scalarProj = dotProduct / magnitudeSquared;
   
    // Create the projection vector (in the direction of vec1)
    Matrix<T> result(vec1.rows, 1);
    for (size_t i = 0; i < vec1.rows; i++) {
        result.mat[i][0] = vec1.mat[i][0] * scalarProj;
    }
   
    return result;
}

// Shearing
template <typename T>
ShearMatrix<T>::ShearMatrix(size_t m, size_t n, T shearX, T shearY)
: Matrix<T>(m, n) {
    // Initialize identity matrix
    for (size_t i = 0; i < m && i < n; i++) {
        this->mat[i][i] = 1;
    }
   
    if (m >= 2 && n >= 2) {
        this->mat[0][1] = shearX;
        this->mat[1][0] = shearY;
    }
}

template <typename T>
void ShearMatrix<T>::transform() {
    cout << "Shearing Matrix:\n";
    this->display();
}

// Rotation
template <typename T>
RotateMatrix<T>::RotateMatrix(T angle)
: Matrix<T>(2, 2) {
    T radians = (PI / 180) * angle;
    this->mat[0][0] = cos(radians);
    this->mat[0][1] = -sin(radians);
    this->mat[1][0] = sin(radians);
    this->mat[1][1] = cos(radians);
}

template <typename T>
void RotateMatrix<T>::transform() {
    cout << "Rotation Matrix:\n";
    this->display();
}

// Scaling
template <typename T>
ScaleMatrix<T>::ScaleMatrix(size_t m, size_t n, T scaleX, T scaleY)
: Matrix<T>(m, n) {
    // Initialize to zeros
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            this->mat[i][j] = 0;
        }
    }
   
    if (m >= 2 && n >= 2) {
        this->mat[0][0] = scaleX;
        this->mat[1][1] = scaleY;
    }
}

template <typename T>
void ScaleMatrix<T>::transform() {
    cout << "Scaling Matrix:\n";
    this->display();
}

// Reflection
template <typename T>
ReflectMatrix<T>::ReflectMatrix(size_t m, size_t n, bool reflectX, bool reflectY)
: Matrix<T>(m, n) {
    // Initialize to zeros first
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            this->mat[i][j] = 0;
        }
    }
   
    // Set diagonal elements
    for (size_t i = 0; i < m && i < n; i++) {
        this->mat[i][i] = 1;
    }
   
    // Apply reflection
    if (m >= 2 && n >= 2) {
        this->mat[0][0] = reflectX ? -1 : 1;
        this->mat[1][1] = reflectY ? -1 : 1;
    }
}

template <typename T>
void ReflectMatrix<T>::transform() {
    cout << "Reflection Matrix:\n";
    this->display();
}

#endif // MATRIX_HPP