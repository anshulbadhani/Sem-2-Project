// solution_types.hpp
#ifndef SOLUTION_TYPES_HPP
#define SOLUTION_TYPES_HPP

#include <string>
#include <vector>
#include <stdexcept> // For std::logic_error
#include <Eigen/Core>
#include <Eigen/Dense>
#include <boost/rational.hpp>
#include <boost/lexical_cast.hpp> // For printing rationals

// Put everything inside the namespace
namespace cas_engine {

// Define Rational type within the namespace or globally before it
using Rational = boost::rational<long long>;

// --- Linear System Solution ---

enum class LinearSolutionType {
    UNIQUE,
    NO_SOLUTION,
    INFINITE_SOLUTIONS,
    ERROR_INVALID_INPUT, // Should primarily be handled by exceptions now
    ERROR_NUMERICAL // For NaN/Inf or severe ill-conditioning issues
};

class LinearSystemSolution {
private:
    LinearSolutionType m_type = LinearSolutionType::ERROR_NUMERICAL; // Default to error if not set
    Eigen::VectorXd m_solutionVector;
    std::string m_message;

public:
    // Default constructor
    LinearSystemSolution() : m_message("Solver not run or invalid state.") {}

    // Constructor for specific results (non-unique solution or error)
    LinearSystemSolution(LinearSolutionType type, std::string message)
        : m_type(type), m_message(std::move(message)) {}

    // Constructor for unique solution
    LinearSystemSolution(Eigen::VectorXd solution, std::string message)
        : m_type(LinearSolutionType::UNIQUE), m_solutionVector(std::move(solution)), m_message(std::move(message)) {}

    // Getters
    LinearSolutionType getStatus() const { return m_type; }
    const std::string& getMessage() const { return m_message; }

    const Eigen::VectorXd& getSolutionVector() const {
        if (m_type != LinearSolutionType::UNIQUE) {
            throw std::logic_error("Solution vector is only available for UNIQUE solutions.");
        }
        return m_solutionVector;
    }

    bool hasUniqueSolution() const {
        return m_type == LinearSolutionType::UNIQUE;
    }
};


// --- Simplex Solution ---

enum class SimplexStatus {
    OPTIMAL,
    INFINITE_SOLUTIONS,
    UNBOUNDED,
    INFEASIBLE, // More specific than NO_SOLUTION for LP (using Two-Phase/BigM)
    ERROR_INPUT, // Should primarily be handled by exceptions now
    ERROR_MAX_ITERATIONS,
    ERROR_INTERNAL // E.g., zero pivot
};

class SimplexSolution {
private:
    SimplexStatus m_status = SimplexStatus::ERROR_INTERNAL; // Default to error
    Rational m_optimalValue = 0;
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> m_variableValues;
    std::string m_message;
    // Optional: Store final tableau/basis for advanced analysis
    // Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic> m_finalTableau;
    // std::vector<int> m_finalBasicVarIndices;

public:
    // Default constructor
    SimplexSolution() : m_message("Solver not run or invalid state.") {}

     // Constructor for statuses without optimal value/variables (e.g., unbounded, error)
    SimplexSolution(SimplexStatus status, std::string message)
        : m_status(status), m_message(std::move(message)) {}

    // Constructor for optimal/infinite solutions
    SimplexSolution(SimplexStatus status, Rational optimalValue,
                    Eigen::Matrix<Rational, Eigen::Dynamic, 1> variableValues,
                    std::string message)
        : m_status(status), m_optimalValue(optimalValue), m_variableValues(std::move(variableValues)), m_message(std::move(message))
    {
        // Ensure status matches expectation
        if (status != SimplexStatus::OPTIMAL && status != SimplexStatus::INFINITE_SOLUTIONS) {
             // This check might be overly strict if called internally carefully, but safer
             throw std::logic_error("Internal Error: Optimal value constructor called with non-optimal status.");
        }
    }


    // Getters
    SimplexStatus getStatus() const { return m_status; }
    const std::string& getMessage() const { return m_message; }

    Rational getOptimalValue() const {
        if (m_status != SimplexStatus::OPTIMAL && m_status != SimplexStatus::INFINITE_SOLUTIONS) {
             throw std::logic_error("Optimal value is only defined for OPTIMAL or INFINITE_SOLUTIONS status.");
        }
        return m_optimalValue;
     }

    const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& getVariableValues() const {
        if (m_status != SimplexStatus::OPTIMAL && m_status != SimplexStatus::INFINITE_SOLUTIONS) {
            throw std::logic_error("Variable values are only meaningful for OPTIMAL or INFINITE_SOLUTIONS status.");
        }
        return m_variableValues;
    }

    bool hasOptimalSolution() const {
        return m_status == SimplexStatus::OPTIMAL || m_status == SimplexStatus::INFINITE_SOLUTIONS;
    }

    // Helper to print rational vector nicely
    static std::string formatVector(const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& vec) {
        std::string out = "[";
        for (Eigen::Index i = 0; i < vec.size(); ++i) {
            try {
                out += boost::lexical_cast<std::string>(vec(i));
            } catch (const boost::bad_lexical_cast& e) {
                out += "err"; // Or some indicator of conversion failure
            }
            if (i < vec.size() - 1) out += ", ";
        }
        out += "]";
        return out;
    }
};


// Helper for absolute value of rational (within namespace)
inline Rational abs_rational(const Rational& r) {
    return boost::abs(r);
}

} // namespace cas_engine

#endif // SOLUTION_TYPES_HPP