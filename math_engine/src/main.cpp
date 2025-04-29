#include <cmath> // std::abs
#include <iostream>
#include <limits>
#include <numeric> // std::gcd
#include <stdexcept>
#include <vector>

#include "utils.hpp"


#include <boost/rational.hpp>

#include <Eigen/Core>
#include <Eigen/Dense> // dynamic matricies and vectors Eigen MatrixXd

/* Things to take care of:
 * 1. Right now we can only find solutions if they are unique
        and throw error for no or infinite solutions.
        We must inform the user whether the solution is
        non-existent or there are infinite number of solutions

*/


// ------ PART-1 SOLVING OF SYSTEM OF LINEAR EQUATIONS USING LU DECOMPOSITION ------ //


// To solve a system of linear equations we have to
// first represent in matrix form: AX = B;
// where X is a vector of unknows which we have to find
// hence the function is of return type VectorXd a dynamic vector type

// and X = inv(A) * B. So params would be A and B

Eigen::VectorXd solveLinearSystem(const Eigen::MatrixXd& A, const Eigen::VectorXd& B) {
    // Calculating the values of inv(A).
    // For that we have to check for invertibility

    if (A.rows() != A.cols()) {
        throw std::runtime_error("The Matrix should be a square matrix");
    }

    if(A.rows() != B.size()) {
        throw std::runtime_error("Every equation must contain a constant term. If none then set it to zero");
    }

    // Using LU partial piviting.
    // This is a method where we decompose a matrix into
    // lower and upper  triangular matricies

    Eigen::PartialPivLU<Eigen::MatrixXd> lu(A);

    // Checking for singularity

    double matrix_norm = A.lpNorm<Eigen::Infinity>();
    double pivoit_tolrence = A.rows() * matrix_norm * std::numeric_limits<double>::epsilon();
    if(pivoit_tolrence == 0.0) {
        pivoit_tolrence = 1e-12;
    }

    Eigen::MatrixXd lu_matrix = lu.matrixLU();
    for (Eigen::Index i = 0; i < A.rows(); ++i) {
        if (std::abs(lu_matrix(i, i)) < pivoit_tolrence) {
            throw std::runtime_error("Matrix A appears to be singular (near zero pivot detected relative to matrix norm).");
        }
    }

    // Solving the equation
    Eigen::VectorXd x = lu.solve(B);

    if (!x.allFinite()) {
        throw std::runtime_error("NaN or Inf solution encounterd. Matrix likely singular or severely ill-conditioned.");

    }


    // --- Step 3: Post-Solve Residual Check ---
    double residual_tolerance = 1e-6; // Example tolerance for solution quality
    if (!B.isZero()) {
        double relative_error = (A * x - B).norm() / B.norm();
        if (relative_error > residual_tolerance) {
             std::cerr << "Warning: High relative error in solution (" << relative_error
                       << "). Solution may be unreliable due to ill-conditioning." << std::endl;
             // Optional: throw std::runtime_error("Solution failed validation (high residual error).");
        }
    } else { // Handle homogeneous case b = 0
         double error_norm = (A*x).norm();
         if (error_norm > residual_tolerance * A.rows()) { // Scale tolerance slightly
              std::cerr << "Warning: High error norm for homogeneous system (" << error_norm << ")." << std::endl;
         }
    }

    // std::cout << "Relative error: " << (A*X-B).norm() / B.norm() << std::endl;

    return x;
}
Eigen::MatrixXd takeInputFromConsole() {
    std::cout << "Enter number of rows: ";
    int rows = 0; int cols = 0;
    while (!(std::cin >> rows) || rows <=0) {
        std::cout << "Invalid input. Please enter a positive integer for rows: ";
        std::cin.clear(); // Clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
    }
    
    std::cout << "Enter number of cols: ";
    while (!(std::cin >> cols) || cols <=0) {
        std::cout << "Invalid input. Please enter a positive integer for columns: ";
        std::cin.clear(); // Clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
    }

    Eigen::MatrixXd m(rows, cols);
    std::cout << "Enter elements of Matrix [" << rows << "][" << cols << "]" << std::endl;

    for (int i = 0; i < rows; ++i) {
        std::cout << "Row: " << i + 1 << ": " << std::endl;
        for (int j = 0; j < cols; ++j) {
            while (!(std::cin >> m(i, j))) { // Use operator() for access
                std::cout << "\nInvalid input for element (" << i + 1 << "," << j + 1 << "). Please enter a number: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }
    return m;
}


// ------ PART-2 OPTIMIZATION USING SIMPLEX ------ //

// setup for simplex

using Rational = boost::rational<long long>;

enum SimplexStatus { OPTIMAL, UNBOUNDED, NO_SOLUTION, ERROR_INPUT, NOT_SOLVED, INFINITE_SOLUTIONS };

class SimplexSolution {
private:
    SimplexStatus m_status = NOT_SOLVED;
    Rational m_optimalValue = 0;
    std::string m_message = "Not solved yet.";
    
private:
    friend SimplexSolution solveSimplex();

public:
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> m_variableValues;

public:
    // constructors /* May needed more? */
    SimplexSolution() = default;

    // getters

    SimplexStatus getStatus() const { return m_status; }

    Rational getOptimalValue() {
        if (m_status != OPTIMAL) throw Warning("Using a non-optimal value.");
        return m_optimalValue;
     }

    const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& getVariableValues() const {
        if (m_status != OPTIMAL || m_status != INFINITE_SOLUTIONS) {
            throw Warning("Accessing variable vaulues when no valid solution exists");
        }
        return m_variableValues;
    }

    const std::string getMessage() const { return m_message; }

    bool hasOptimalSolution() const {
        return m_status == OPTIMAL;
    }

    bool hasSolution() const {
        return m_status == OPTIMAL || m_status == INFINITE_SOLUTIONS;
    }

    // setter functions

    void setStatus(SimplexStatus status) { m_status = status; }

    void setMessage(std::string message) { m_message = message; }

    void setVariableValuesToZero() {
        m_variableValues.setZero();
    }

};


Rational abs_rational (const Rational& r) {
    return boost::abs(r);
}

// solver

SimplexSolution solveSimplex(
    const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& objectiveCoefficients, // Vector c for Max Z = c*x
    const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& constraintMatrix, // Matrix A for Ax <= b
    const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& constraintRHS) // Vector b for Ax <= b 

{
    SimplexSolution result;
    int numOriginalVars = objectiveCoefficients.size();
    int numConstraints = constraintMatrix.rows();

    // --- Basic Input Validation ---
    if (constraintMatrix.cols() != numOriginalVars) {
        result.setMessage("Error: Number of columns in constraint matrix must match number of objective coefficients.");
        throw std::runtime_error(result.getMessage());
    }
    
    if (constraintRHS.size() != numConstraints) {
        result.setMessage("Error: Number of elements in RHS vector must match number of constraints.");
        throw std::runtime_error(result.getMessage());
    }

    for (int i = 0; i < numConstraints; ++i) {
        if (constraintRHS(i) < 0) {
            // This simple implementation requires non-negative RHS
            result.setMessage("Error: This implementation requires non-negative RHS values (b_i >= 0).");
            throw std::runtime_error(result.getMessage());
        }
    }

    // Making the initial Tableau

    int numSlackVars = numConstraints;
    int numTotalVars = numOriginalVars + numSlackVars;
    int numTableauRows = numConstraints + 1; // +1 for objective row
    int numTableauCols = numTotalVars + 1;   // +1 for RHS column

    Eigen::Matrix<Rational, Eigen::Dynamic, 1> tableau(numTableauRows, numTableauCols);
    tableau.setZero();

    // Fill constraint rows (A | I | b)
    tableau.block(0, 0, numConstraints, numOriginalVars) = constraintMatrix; // Copy A
    tableau.block(0, numOriginalVars, numConstraints, numSlackVars).setIdentity(); // Identity for slacks
    tableau.block(0, numTotalVars, numConstraints, 1) = constraintRHS; // Copy b (RHS)

    tableau.block(numTableauRows - 1, 0, 1, numOriginalVars) = -objectiveCoefficients.transpose(); // Note the negation and transpose


        // Keep track of which variable is basic for each row (initially the slack vars)
    // Index corresponds to the column index in the tableau
    std::vector<int> basicVarIndices(numConstraints);
    for(int i = 0; i < numConstraints; ++i) {
        basicVarIndices[i] = numOriginalVars + i; // Slack variable S_i is basic in row i
    }


    // --- Simplex Iterations (Pivoting) ---
    int iteration = 0;
    const int MAX_ITERATIONS = 1000; // Safety break for potential cycles

    while (iteration++ < MAX_ITERATIONS) {

        // 1. Find Pivot Column (Entering Variable)
        int pivotCol = -1;
        Rational minObjectiveCoeff = 0;
        for (int j = 0; j < numTotalVars; ++j) { // Check objective row coeffs (excluding RHS)
            if (tableau(numTableauRows - 1, j) < minObjectiveCoeff) {
                minObjectiveCoeff = tableau(numTableauRows - 1, j);
                pivotCol = j;
            }
        }

        // Check for Optimality
        if (pivotCol == -1) {
            result.setStatus(OPTIMAL);
            result.setMessage("Optimal solution found.");
            break; // Optimal!
        }

        // 2. Find Pivot Row (Leaving Variable - Minimum Ratio Test)
        int pivotRow = -1;
        Rational minRatio = -1; // Use -1 to indicate not yet found

        for (int i = 0; i < numConstraints; ++i) { // Iterate through constraint rows
            Rational element = tableau(i, pivotCol);
            if (element > 0) { // Must be positive for ratio test
                Rational rhs = tableau(i, numTotalVars); // RHS of this row
                Rational ratio = rhs / element;

                if (pivotRow == -1 || ratio < minRatio) {
                    minRatio = ratio;
                    pivotRow = i;
                }
            }
        }

        // Check for Unboundedness
        if (pivotRow == -1) {
            result.setStatus(UNBOUNDED);
            result.setMessage("Problem is unbounded.");
            break; // Unbounded!
        }

        // 3. Perform Pivot Operation
        // a) Normalize the pivot row
        Rational pivotElement = tableau(pivotRow, pivotCol);
        tableau.row(pivotRow) /= pivotElement;

        // b) Eliminate other entries in pivot column
        for (int i = 0; i < numTableauRows; ++i) {
            if (i != pivotRow) {
                Rational factor = tableau(i, pivotCol);
                tableau.row(i) -= factor * tableau.row(pivotRow);
            }
        }

        // Update which variable is basic in the pivot row
        basicVarIndices[pivotRow] = pivotCol;

        // std::cout << "--- Iteration " << iteration << " ---\n" << tableau << std::endl; // Debug print

    } // End of while loop (iterations)

    if (iteration >= MAX_ITERATIONS) {
         result.setMessage("Error: Maximum iterations reached, potential cycle or slow convergence.");
    }

    // --- Extract Solution (if Optimal) ---
    if (result.getStatus() == OPTIMAL) {
        result.getOptimalValue() = tableau(numTableauRows - 1, numTotalVars); // Bottom-right corner

        // Initialize variable values to zero
        result.m_variableValues = Eigen::Matrix<Rational, Eigen::Dynamic, 1>(numOriginalVars);
        result.setVariableValuesToZero();

        // Assign values for basic variables that are original variables
        for(int i=0; i < numConstraints; ++i) {
            int basicVarIndex = basicVarIndices[i];
            if (basicVarIndex < numOriginalVars) { // Check if it's an original variable (not slack)
                result.m_variableValues(basicVarIndex) = tableau(i, numTotalVars); // Value is in RHS column
            }
        }
    }
    return result;
}

int main() {
    // Eigen::VectorXd B(2);
    // // taking vector as input
    // B << 3, 4;
    // Eigen::MatrixXd A = takeInputFromConsole();

    // Eigen::VectorXd x = solveLinearSystem(A, B);

    // std::cout << "The solution is: " << std::endl;
    // for (int i = 0; i < x.size(); ++i) {
    //     std::cout << "x" << i+1 << ": " << x[i] << std::endl;
    // }

        // Example Problem: Maximize Z = 3x1 + 5x2
    // Subject to:
    // x1 <= 4         (1*x1 + 0*x2 <= 4)
    // 2x2 <= 12        (0*x1 + 2*x2 <= 12)
    // 3x1 + 2x2 <= 18  (3*x1 + 2*x2 <= 18)
    // x1, x2 >= 0

    int numVars = 2;
    int numConstraints = 3;

    // Objective Coefficients (c)
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> c(numVars);
    c << Rational(3), Rational(5);

    // Constraint Matrix (A)
    Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic> A(numConstraints, numVars);
    A << Rational(1), Rational(0),
         Rational(0), Rational(2),
         Rational(3), Rational(2);

    // Constraint RHS (b)
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> b(numConstraints);
    b << Rational(4), Rational(12), Rational(18);

    // Solve the LP
    SimplexSolution result = solveSimplex(c, A, b);

    // Print results
    std::cout << "--- Simplex Result ---" << std::endl;
    std::cout << "Status: " << result.getMessage() << std::endl;

    if (result.getStatus() == OPTIMAL) {
        std::cout << "Optimal Objective Value (Z_max): " << result.getOptimalValue() << " (" << result.getOptimalValue().toDouble() << ")" << std::endl;
        std::cout << "Variable Values:" << std::endl;
        for (int i = 0; i < result.m_variableValues.size(); ++i) {
            std::cout << "  x" << i + 1 << " = " << result.m_variableValues(i) << " (" << result.m_variableValues.toDouble() << ")" << std::endl;
        }
         // If you were minimizing Cost = -Z, the minimum cost is -result.optimalValue
    }

    return 0;
}