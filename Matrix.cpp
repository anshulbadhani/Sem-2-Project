#include <iostream>
#include <cmath>  // For rotation (cos, sin)
using namespace std;

const double PI = 3.14159265358979323846;

// Template class for MxN matrix
template <typename T>
class Matrix {
protected:
    size_t rows, cols;// Matrix dimensions MxN
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

    virtual void transform(){}

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

    void transform() override {
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

    void transform() override {
        cout << "Rotation Matrix (2x2 only):" << endl;
        this->display();
    }
};

// Derived class for Addition (only for same-sized matrices MxN)
template <typename T>
class AddMatrix : public Matrix<T> {
public:
    AddMatrix(const Matrix<T>& m1, const Matrix<T>& m2) : Matrix<T>(m1.getRows(), m1.getCols()) {
        if (m1.getRows() != m2.getRows() || m1.getCols() != m2.getCols()) {
            cout << "Error: Matrix sizes do not match for addition!" << endl;
            return;
        }
        for (size_t i = 0; i < this->getRows(); i++) {
            for (size_t j = 0; j < this->getCols(); j++) {
                this->mat[i][j] = m1.getElement(i, j) + m2.getElement(i, j);
            }
        }
    }

    void transform() override {
        cout << "Addition Result:" << endl;
        this->display();
    }
};

// Derived class for Subtraction (only for same-sized matrices MxN)
template <typename T>
class SubtractMatrix : public Matrix<T> {
public:
    SubtractMatrix(const Matrix<T>& m1, const Matrix<T>& m2) : Matrix<T>(m1.getRows(), m1.getCols()) {
        if (m1.getRows() != m2.getRows() || m1.getCols() != m2.getCols()) {
            cout << "Error: Matrix sizes do not match for subtraction!" << endl;
            return;
        }
        for (size_t i = 0; i < this->getRows(); i++) {
            for (size_t j = 0; j < this->getCols(); j++) {
                this->mat[i][j] = m1.getElement(i, j) - m2.getElement(i, j);
            }
        }
    }

    void transform() override {
        cout << "Subtraction Result:" << endl;
        this->display();
    }
};

int main() {

    size_t M, N;
    cout << "Enter number of rows (M): ";
    cin >> M;
    cout << "Enter number of columns (N): ";
    cin >> N;

    // Use template for different data types (int, float, double, etc.)
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
                if (M == mat2->getRows() && N == mat2->getCols()) {
                    Matrix<double>* add = new AddMatrix<double>(*mat1, *mat2);
                    add->transform();
                    delete add;
                } else {
                    cout << "Addition requires matrices of the same size.\n";
                }
                break;
            }
            case 4: {
                if (M == mat2->getRows() && N == mat2->getCols()) {
                    Matrix<double>* subtract = new SubtractMatrix<double>(*mat1, *mat2);
                    subtract->transform();
                    delete subtract;
                } else {
                    cout << "Subtraction requires matrices of the same size.\n";
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