#include <iostream>
#include <cmath>  // For rotation (cos, sin)
using namespace std;

const double PI = 3.14159265358979323846;

// Template class for MxN matrix
template <typename T>
class Matrix {
protected:
    size_t rows, cols; // Matrix dimensions MxN
    T** mat;           // Dynamic allocation

public:
    Matrix(size_t m, size_t n) : rows(m), cols(n) {
        mat = new T*[rows];
        for (size_t i = 0; i < rows; i++) {
            mat[i] = new T[cols]{0}; // Initialize with zeros
        }
    }

    virtual ~Matrix() {
        for (size_t i = 0; i < rows; i++) {
            delete[] mat[i];
        }
        delete[] mat;
    }

    void inputMatrix() {
        cout << "Enter elements of " << rows << "x" << cols << " matrix (row-wise):\n";
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                cin >> mat[i][j];
            }
        }
    }

    void display() const {
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                cout << mat[i][j] << " ";
            }
            cout << endl;
        }
    }

    T getElement(size_t i, size_t j) const {
        return mat[i][j];
    }

    void setElement(size_t i, size_t j, T value) {
        mat[i][j] = value;
    }

    size_t getRows() const {
        return rows;
    }

    size_t getCols() const {
        return cols;
    }

    // Overloaded operator for matrix addition
    Matrix<T> operator+(const Matrix<T>& other) const {
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

    // Overloaded operator for matrix subtraction
    Matrix<T> operator-(const Matrix<T>& other) const {
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
};

// Derived class for Shearing (only valid for at least 2x2 matrices)
template <typename T>
class ShearMatrix : public Matrix<T> {
public:
    ShearMatrix(size_t m, size_t n, T shearX, T shearY) : Matrix<T>(m, n) {
        if (m >= 2 && n >= 2) { // Apply shearing for at least 2x2 matrices
            this->mat[0][1] = shearX;
            this->mat[1][0] = shearY;
        }
    }

    void transform() {
        cout << "Shearing Matrix:" << endl;
        this->display();
    }
};

// Derived class for Rotation (only valid for 2x2 matrices)
template <typename T>
class RotateMatrix : public Matrix<T> {
public:
    RotateMatrix(T angle) : Matrix<T>(2, 2) {
        T radians = (PI / 180) * angle;
        this->mat[0][0] = cos(radians); this->mat[0][1] = -sin(radians);
        this->mat[1][0] = sin(radians); this->mat[1][1] = cos(radians);
    }

    void transform() {
        cout << "Rotation Matrix (2x2 only):" << endl;
        this->display();
    }
};

int main() {
    size_t M, N;
    cout << "Enter number of rows (M): ";
    cin >> M;
    cout << "Enter number of columns (N): ";
    cin >> N;

    Matrix<double>* mat1 = new Matrix<double>(M, N);
    Matrix<double>* mat2 = new Matrix<double>(M, N);

    cout << "Enter first matrix:\n";
    mat1->inputMatrix();

    cout << "Enter second matrix:\n";
    mat2->inputMatrix();

    int choice;
    do {
        cout << "\nChoose an operation to perform:\n";
        cout << "1. Shear Transformation\n";
        cout << "2. Rotation (only for 2x2 matrices)\n";
        cout << "3. Matrix Addition\n";
        cout << "4. Matrix Subtraction\n";
        cout << "5. Display Matrices\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                if (M >= 2 && N >= 2) {
                    double shearX, shearY;
                    cout << "Enter shear factors (X and Y): ";
                    cin >> shearX >> shearY;
                    Matrix<double>* shear = new ShearMatrix<double>(M, N, shearX, shearY);
                    shear->transform();
                    delete shear;
                } else {
                    cout << "Shearing requires at least a 2x2 matrix.\n";
                }
                break;
            }
            case 2: {
                if (M == 2 && N == 2) {
                    double angle;
                    cout << "Enter rotation angle (degrees): ";
                    cin >> angle;
                    Matrix<double>* rotate = new RotateMatrix<double>(angle);
                    rotate->transform();
                    delete rotate;
                } else {
                    cout << "Rotation is only implemented for 2x2 matrices.\n";
                }
                break;
            }
            case 3: {
                try {
                    Matrix<double> result = *mat1 + *mat2;
                    cout << "Addition Result:\n";
                    result.display();
                } catch (const runtime_error& e) {
                    cout << e.what() << endl;
                }
                break;
            }
            case 4: {
                try {
                    Matrix<double> result = *mat1 - *mat2;
                    cout << "Subtraction Result:\n";
                    result.display();
                } catch (const runtime_error& e) {
                    cout << e.what() << endl;
                }
                break;
            }
            case 5: {
                cout << "\nFirst Matrix:\n";
                mat1->display();
                cout << "\nSecond Matrix:\n";
                mat2->display();
                break;
            }
            case 6:
                cout << "Exiting program...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 6);

    delete mat1;
    delete mat2;

    return 0;
}
