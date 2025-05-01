// linear_solver.cpp
#include "linear_solver.hpp"
#include <Eigen/QR> // For ColPivHouseholderQR
#include <limits>   // For numeric_limits
#include <cmath>    // For std::abs
#include <stdexcept> // For std::invalid_argument
#include <string>   // For std::to_string

// No iostream needed here if warnings are added to the message string

namespace cas_engine {

LinearSystemSolution LinearSolver::solve(const Eigen::MatrixXd& A, const Eigen::VectorXd& B) const
{
    // --- Pre-computation Input Validation ---
    if (A.rows() <= 0 || A.cols() <= 0) {
        throw std::invalid_argument("LinearSolver Error: Matrix A dimensions must be positive.");
    }
     if (B.size() <= 0) {
         throw std::invalid_argument("LinearSolver Error: Vector B size must be positive.");
     }
    if (A.rows() != A.cols()) {
        throw std::invalid_argument("LinearSolver Error: Matrix A must be square.");
    }
    if (A.rows() != B.size()) {
        throw std::invalid_argument("LinearSolver Error: Dimension mismatch - A.rows() ["
            + std::to_string(A.rows()) + "] must equal B.size() ["
            + std::to_string(B.size()) + "].");
    }
     if (!A.allFinite() || !B.allFinite()) {
         throw std::invalid_argument("LinearSolver Error: Input matrix A or vector B contains NaN or Inf.");
     }


    int n = A.rows(); // Number of variables/equations

    // --- Rank Computation using QR Decomposition ---
    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr(A);
    // Consider setting a threshold explicitly if needed:
    // double threshold = n * A.norm() * std::numeric_limits<double>::epsilon();
    // qr.setThreshold(threshold);
    // qrAugmented.setThreshold(threshold); // Use same threshold
    Eigen::Index rankA = qr.rank();

    Eigen::MatrixXd augmentedMatrix(n, n + 1);
    augmentedMatrix << A, B;
    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrAugmented(augmentedMatrix);
    Eigen::Index rankAugmented = qrAugmented.rank();

    // --- Determine Solution Type and Solve ---
    if (rankA == n) {
        // Rank(A) == n implies unique solution for square A
        Eigen::VectorXd x = qr.solve(B);

        // Post-Solve Numerical Stability Check
        if (!x.allFinite()) {
             // Return error status for numerical issues during solve
            return {LinearSolutionType::ERROR_NUMERICAL,
                    "Numerical Error: Solution contains NaN or Inf. Matrix might be severely ill-conditioned."};
        }

        // Residual check (optional but recommended)
        double residual_tolerance = 1e-6; // Example tolerance
        double residual_error_norm = 0.0;
        double b_norm = B.norm();

        // Avoid division by zero for B=0 case
        if (b_norm > std::numeric_limits<double>::epsilon()) { // Use epsilon for safe comparison
             residual_error_norm = (A * x - B).norm() / b_norm;
        } else {
            // For homogeneous systems (B=0), check norm of A*x directly
             double ax_norm = (A * x).norm();
             double a_norm = A.norm(); // Frobenius norm by default
             // Relative error check is tricky for Ax=0. Check if Ax is small relative to A's scale.
             if (a_norm > std::numeric_limits<double>::epsilon()){
                  residual_error_norm = ax_norm / a_norm; // Compare relative to A's norm
             } else {
                  residual_error_norm = ax_norm; // If A is zero matrix, check absolute norm
             }

             // Adjust tolerance slightly for homogeneous case if desired
             // residual_tolerance *= n; // Example adjustment
        }


        std::string message = "Unique solution found.";
        if (residual_error_norm > residual_tolerance) {
             // Append warning to the message instead of printing to cerr directly
             message += " Warning: High relative residual ("
                      + std::to_string(residual_error_norm)
                      + ") suggests potential numerical instability or ill-conditioning.";
             // Optionally, could change status to ERROR_NUMERICAL if residual is very high
             // return {LinearSolutionType::ERROR_NUMERICAL, message};
        }

        return {x, message}; // Use constructor for unique solution

    } else { // rankA < n (Matrix A is singular)
        if (rankA < rankAugmented) {
            // rank(A) < rank([A | B]) -> No solution (inconsistent system)
            return {LinearSolutionType::NO_SOLUTION, "No solution exists (inconsistent system - rank(A) < rank([A|B]))."};
        } else {
            // rank(A) == rank([A | B]) < n -> Infinite solutions
            return {LinearSolutionType::INFINITE_SOLUTIONS, "Infinite solutions exist (rank(A) == rank([A|B]) < n)."};
            // Note: Finding the general form requires null space calculation (more advanced)
        }
    }
}

} // namespace cas_engine