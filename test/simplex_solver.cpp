// simplex_solver.cpp
#include "simplex_solver.hpp"
#include <numeric> // std::iota
#include <limits>
#include <stdexcept> // For std::invalid_argument
#include <vector>
#include <string>   // For std::to_string
#include <boost/lexical_cast.hpp> // For error messages

namespace cas_engine {

SimplexSolution SimplexSolver::solve(
    const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& c, // objectiveCoefficients
    const Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic>& A, // constraintMatrix
    const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& b) const // constraintRHS
{
    // --- Input Validation ---
    if (c.size() <= 0) {
        throw std::invalid_argument("SimplexSolver Error: Objective coefficient vector c cannot be empty.");
    }
     if (A.rows() <= 0 || A.cols() <= 0) {
         throw std::invalid_argument("SimplexSolver Error: Constraint matrix A dimensions must be positive.");
     }
     if (b.size() <= 0) {
         throw std::invalid_argument("SimplexSolver Error: RHS vector b cannot be empty.");
     }


    int numOriginalVars = c.size();
    int numConstraints = A.rows();

    if (A.cols() != numOriginalVars) {
        throw std::invalid_argument("SimplexSolver Error: Constraint matrix A columns ("
            + std::to_string(A.cols()) + ") must match objective coefficients size ("
            + std::to_string(numOriginalVars) + ").");
    }
    if (b.size() != numConstraints) {
         throw std::invalid_argument("SimplexSolver Error: Constraint RHS vector b size ("
            + std::to_string(b.size()) + ") must match constraint matrix A rows ("
            + std::to_string(numConstraints) + ").");
    }
    // Check for non-negative RHS (limitation of this simple implementation)
    for (int i = 0; i < numConstraints; ++i) {
        if (b(i) < 0) {
            // This simple version doesn't handle negative RHS. Requires Two-Phase/Big-M.
             throw std::invalid_argument("SimplexSolver Error: This implementation requires non-negative RHS (b>=0). Constraint "
                + std::to_string(i+1) + " has RHS " + boost::lexical_cast<std::string>(b(i)) + ".");
        }
    }
    // Could add checks for NaN/Inf if using floating point types

    // --- Setup Initial Tableau ---
    int numSlackVars = numConstraints;
    int numTotalVars = numOriginalVars + numSlackVars;
    int numTableauRows = numConstraints + 1; // +1 for objective row (Z)
    int numTableauCols = numTotalVars + 1;   // +1 for RHS column (value)

    Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic> tableau(numTableauRows, numTableauCols);
    tableau.setZero();

    // Fill constraint rows: [ A | I | b ]
    tableau.block(0, 0, numConstraints, numOriginalVars) = A;
    tableau.block(0, numOriginalVars, numConstraints, numSlackVars).setIdentity();
    tableau.block(0, numTotalVars, numConstraints, 1) = b;

    // Fill objective row (Z row): [ -c^T | 0 | 0 ]
    tableau.block(numConstraints, 0, 1, numOriginalVars) = -c.transpose();

    // Track basic variables (column indices). Initially slack variables.
    std::vector<int> basicVarIndices(numConstraints);
    std::iota(basicVarIndices.begin(), basicVarIndices.end(), numOriginalVars); // Fills with numOriginalVars, numOriginalVars+1, ...

    // --- Simplex Iterations ---
    int iteration = 0;
    while (iteration++ < m_options.max_iterations) {

        // 1. Find Pivot Column (Entering Variable) - Most negative in Z row
        int pivotCol = -1;
        Rational minObjectiveCoeff = 0; // Using 0 as the threshold (exact comparison with Rational)
        for (int j = 0; j < numTotalVars; ++j) { // Iterate through variable columns
            if (tableau(numConstraints, j) < minObjectiveCoeff) {
                minObjectiveCoeff = tableau(numConstraints, j);
                pivotCol = j;
            }
        }

        // 2. Check for Optimality
        if (pivotCol == -1) { // No negative coefficient in Z row -> Optimal (or infinitely optimal)
            // Extract final solution details before checking for infinite solutions
            Rational optimal_val = tableau(numConstraints, numTotalVars);
            Eigen::Matrix<Rational, Eigen::Dynamic, 1> solution_vec =
                Eigen::Matrix<Rational, Eigen::Dynamic, 1>::Zero(numOriginalVars);

            for(int i = 0; i < numConstraints; ++i) { // Iterate through basic variables (rows)
                int basic_var_idx = basicVarIndices[i];
                if (basic_var_idx < numOriginalVars) { // If original variable is basic
                    solution_vec(basic_var_idx) = tableau(i, numTotalVars); // Its value is in RHS
                }
            }
            // Non-basic original variables are implicitly zero

             // Check for Infinite Solutions (non-basic variable with zero cost in Z row)
             bool infinite_solutions_detected = false;
             for (int j = 0; j < numTotalVars; ++j) { // Iterate through all variable columns
                 // Check if variable j is non-basic
                 bool is_basic = false;
                 for(int basic_idx_in_row : basicVarIndices) {
                      if (j == basic_idx_in_row) {
                          is_basic = true;
                          break;
                      }
                 }
                 // If j is non-basic AND its objective coefficient is exactly zero
                 if (!is_basic && tableau(numConstraints, j) == 0) {
                     infinite_solutions_detected = true;
                     // Check if this non-basic variable *could* enter the basis
                     // (i.e., if its column has at least one positive entry for ratio test)
                     // This is a stronger check, but the zero coeff alone is sufficient indication
                     break; // Found one, no need to check further
                 }
             }

            if (infinite_solutions_detected) {
                 return {SimplexStatus::INFINITE_SOLUTIONS, optimal_val, solution_vec,
                         "Optimal solution found, but infinite solutions exist (zero objective coefficient for a non-basic variable)."};
            } else {
                 return {SimplexStatus::OPTIMAL, optimal_val, solution_vec,
                         "Optimal solution found."};
            }
            // Break is implicit due to return
        }

        // 3. Find Pivot Row (Leaving Variable - Minimum Ratio Test)
        int pivotRow = -1;
        Rational minRatio(-1); // Sentinel for not found, must be comparable (<) to positive rationals

        for (int i = 0; i < numConstraints; ++i) { // Iterate through constraint rows
            Rational element = tableau(i, pivotCol);
            // Ratio test only applies if the element in the pivot column is strictly positive.
            if (element > 0) {
                Rational rhs = tableau(i, numTotalVars); // RHS of this row
                 // If RHS is 0 and element is positive, ratio is 0. This is valid & often chosen first.
                Rational ratio = rhs / element;

                // If this is the first valid positive ratio OR it's smaller than the current minRatio
                // Handle the initial case where minRatio is -1
                if (pivotRow == -1 || ratio < minRatio) {
                    minRatio = ratio;
                    pivotRow = i;
                }
                // TODO: Add tie-breaking rule (e.g., Bland's rule using smallest index)
                // if ratio == minRatio, choose row 'i' if variable basicVarIndices[i] has smaller index?
            }
        }

        // 4. Check for Unboundedness
        if (pivotRow == -1) {
            // If a pivot column was found, but no positive element exists in that column
            // for the ratio test (all are <= 0), the problem is unbounded.
            return {SimplexStatus::UNBOUNDED,
                    "Problem is unbounded (entering variable in column " + std::to_string(pivotCol)
                    + " can increase indefinitely)."};
            // Break is implicit
        }

        // 5. Perform Pivot Operation (Gauss-Jordan Elimination Step)
        Rational pivotElement = tableau(pivotRow, pivotCol);
         if (pivotElement <= 0) { // Should not happen based on pivot row selection logic
              // Added <= 0 check just in case, although >0 was required to select pivotRow
              return {SimplexStatus::ERROR_INTERNAL, "Internal Error: Pivot element is non-positive ("
                        + boost::lexical_cast<std::string>(pivotElement) + ") at row "
                        + std::to_string(pivotRow) + ", col " + std::to_string(pivotCol) + "."};
         }

        // a) Normalize the pivot row
        tableau.row(pivotRow) /= pivotElement;

        // b) Eliminate other entries in pivot column
        for (int i = 0; i < numTableauRows; ++i) { // Iterate through all rows including Z row
            if (i != pivotRow) {
                Rational factor = tableau(i, pivotCol); // Element to eliminate
                if (factor != 0) { // Avoid unnecessary computation if already zero
                    tableau.row(i) -= factor * tableau.row(pivotRow);
                }
            }
        }

        // Update the basic variable for the pivot row
        basicVarIndices[pivotRow] = pivotCol;

    } // End of while loop

    // If loop finishes because max iterations were reached
    return {SimplexStatus::ERROR_MAX_ITERATIONS,
            "Maximum iterations (" + std::to_string(m_options.max_iterations) + ") reached. Check for cycling or increase limit."};
}

} // namespace cas_engine