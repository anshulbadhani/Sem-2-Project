// linear_solver.hpp
#ifndef LINEAR_SOLVER_HPP
#define LINEAR_SOLVER_HPP

#include <Eigen/Core>
#include <Eigen/Dense>
#include "solution_types.hpp" // Include the result structure definition (needs to be in namespace)

namespace cas_engine { // Put library code in a namespace

class LinearSolver {
public:
    // Constructor (can be default, state is managed per-solve call)
    LinearSolver() = default;

    // The main solving method
    // Throws std::invalid_argument for invalid input shapes/content (NaN/Inf)
    // Returns LinearSystemSolution indicating status and result/message
    LinearSystemSolution solve(const Eigen::MatrixXd& A, const Eigen::VectorXd& B) const;

private:
    // You could add private helper methods if the solving process becomes complex
    // e.g., check_dimensions, calculate_ranks, perform_residual_check
};

} // namespace cas_engine

#endif // LINEAR_SOLVER_HPP