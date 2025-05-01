// simplex_solver.hpp
#ifndef SIMPLEX_SOLVER_HPP
#define SIMPLEX_SOLVER_HPP

#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include <string>
#include "solution_types.hpp" // Include Rational, SimplexSolution, etc. (needs namespace)

namespace cas_engine {

// Define Options struct *before* the SimplexSolver class
struct SimplexSolverOptions {
    int max_iterations = 1000;
    // Could add tolerance if using floating point, or flags for pivot rules
    // e.g., Rational pivot_tolerance = Rational(1, 1000000);
};


class SimplexSolver {
public:
    // Use the pre-defined struct name for options
    explicit SimplexSolver(SimplexSolverOptions options = SimplexSolverOptions{})
        : m_options(options) {}

    // Solve method for standard form: Maximize c*x subject to Ax <= b, x >= 0
    // Throws std::invalid_argument for invalid input shapes or negative RHS
    // Returns SimplexSolution indicating status and result/message
    SimplexSolution solve(
        const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& objectiveCoefficients_c,
        const Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic>& constraintMatrix_A,
        const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& constraintRHS_b) const;

    // Could add overloads for minimization or different constraint types later

private:
    SimplexSolverOptions m_options; // Use the pre-defined struct name

    // Internal helper methods can be added here if desired:
    // - validate_input(...)
    // - setup_initial_tableau(...)
    // - find_pivot_column(...)
    // - find_pivot_row(...)
    // - perform_pivot(...)
    // - check_for_infinite_solutions(...)
    // - extract_solution(...)
};

} // namespace cas_engine

#endif // SIMPLEX_SOLVER_HPP