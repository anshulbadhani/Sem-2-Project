// #include <iostream>
// #include <vector>
// #include <limits>   // For numeric_limits
// #include <stdexcept> // To catch exceptions from solvers
// #include <iomanip>  // For std::setprecision

// // Include the necessary headers for the classes we created
// #include "solution_types.hpp" // Defines types IN the namespace now
// #include "linear_solver.hpp"  // Defines solver IN the namespace
// #include "simplex_solver.hpp" // Defines solver IN the namespace

// // Bring namespace into scope for convenience in main, or qualify types fully
// using namespace cas_engine;

// // Function to get matrix input (basic example with validation)
// Eigen::MatrixXd getMatrixInput(int rows, int cols) {
//      Eigen::MatrixXd m(rows, cols);
//      std::cout << "Enter elements for (" << rows << "x" << cols << ") matrix row by row:" << std::endl;
//      for (int i = 0; i < rows; ++i) {
//          std::cout << "Row " << i + 1 << ": ";
//          for (int j = 0; j < cols; ++j) {
//              // Loop until valid input is received for the element
//              while (!(std::cin >> m(i, j))) {
//                  std::cout << "\nInvalid input for (" << i + 1 << "," << j + 1 << "). Please enter a number: ";
//                  std::cin.clear(); // Clear error flags
//                  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
//              }
//          }
//      }
//      return m;
// }

// // Function to get vector input (basic example with validation)
// Eigen::VectorXd getVectorInput(int size) {
//     Eigen::VectorXd v(size);
//     std::cout << "Enter the " << size << " elements for the vector:" << std::endl;
//     for (int i = 0; i < size; ++i) {
//         std::cout << "Element " << i + 1 << ": ";
//         while (!(std::cin >> v(i))) {
//              std::cout << "\nInvalid input. Please enter a number: ";
//              std::cin.clear();
//              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//          }
//     }
//     return v;
// }


// // Helper function to print linear system results
// void printLinearResult(const LinearSystemSolution& result) {
//     std::cout << "\n--- Linear System Result ---" << std::endl;
//     std::cout << "Status: " << result.getMessage() << std::endl;
//     // Set precision for printing double results
//     std::cout << std::fixed << std::setprecision(6);
//     if (result.hasUniqueSolution()) {
//         try {
//             std::cout << "Solution Vector X:\n" << result.getSolutionVector().transpose() << std::endl;
//         } catch (const std::logic_error& e) {
//              // Should not happen if hasUniqueSolution is true, but good practice
//              std::cerr << "Error accessing solution vector: " << e.what() << std::endl;
//         }
//     } else if (result.getStatus() == LinearSolutionType::INFINITE_SOLUTIONS) {
//         std::cout << "(System has infinitely many solutions)" << std::endl;
//     } else if (result.getStatus() == LinearSolutionType::NO_SOLUTION) {
//         std::cout << "(System has no solution)" << std::endl;
//     } else {
//          std::cout << "(Solver reported an error or numerical issue)" << std::endl;
//     }
//     // Reset precision to default
//     std::cout.unsetf(std::ios_base::floatfield);
//     std::cout << std::setprecision(6); // Default precision
// }

// // Helper function to print simplex results
// void printSimplexResult(const SimplexSolution& result) {
//      std::cout << "\n--- Simplex Result ---" << std::endl;
//      std::cout << "Status: " << result.getMessage() << std::endl;

//      if (result.hasOptimalSolution()) {
//          try {
//              // Print rational number directly using Boost's stream operator (if available/correctly included)
//              // Or use lexical_cast as before
//              std::cout << "Optimal Value: " << result.getOptimalValue() << std::endl;
//              // Can also try converting to double for easier viewing, but loses precision
//              // std::cout << "Optimal Value (double approx): " << boost::rational_cast<double>(result.getOptimalValue()) << std::endl;

//              std::cout << "Variable Values: " << SimplexSolution::formatVector(result.getVariableValues()) << std::endl;
//              if (result.getStatus() == SimplexStatus::INFINITE_SOLUTIONS) {
//                   std::cout << "(Note: One of potentially infinite optimal solutions shown)" << std::endl;
//              }
//          } catch (const std::logic_error& e) {
//              std::cerr << "Error accessing result properties: " << e.what() << std::endl;
//          } catch (const boost::bad_lexical_cast& e) {
//              std::cerr << "Error formatting rational number for display: " << e.what() << std::endl;
//          } 
//         //  catch (const boost::bad_rational_cast& e) {
//         //      std::cerr << "Error casting rational number for display: " << e.what() << std::endl;
//         //  }
//      } else if (result.getStatus() == SimplexStatus::UNBOUNDED) {
//          std::cout << "(Objective function is unbounded)" << std::endl;
//      } else {
//          std::cout << "(No optimal solution found due to input error, infeasibility, max iterations, or other issue)" << std::endl;
//      }
// }


// int main() {
//     // --- Linear Solver Usage ---
//     std::cout << "------ Linear System Solver (Ax = B) ------" << std::endl;
//     int n_eq = 0;
//     std::cout << "Enter number of equations/variables (must be > 0): ";
//     while (!(std::cin >> n_eq) || n_eq <= 0) {
//         std::cout << "Invalid input. Please enter a positive integer: ";
//         std::cin.clear();
//         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//     }


//     try {
//         Eigen::MatrixXd A = getMatrixInput(n_eq, n_eq);
//         Eigen::VectorXd B = getVectorInput(n_eq);

//         LinearSolver linear_solver; // Use the class from the namespace
//         LinearSystemSolution lin_result = linear_solver.solve(A, B); // Call solve method
//         printLinearResult(lin_result);

//     } catch (const std::invalid_argument& e) {
//         std::cerr << "\nInput Error for Linear Solver: " << e.what() << std::endl;
//     } catch (const std::exception& e) { // Catch other potential errors (e.g., std::logic_error from getters)
//         std::cerr << "\nAn unexpected error occurred during linear solving: " << e.what() << std::endl;
//     }


//     // --- Simplex Solver Usage Example ---
//      std::cout << "\n------ Simplex Solver Example (Max 3x1 + 5x2) ------" << std::endl;
//      std::cout << "Solving: Maximize Z = 3x1 + 5x2" << std::endl;
//      std::cout << "Subject to:" << std::endl;
//      std::cout << "  x1        <= 4" << std::endl;
//      std::cout << "      2x2   <= 12" << std::endl;
//      std::cout << "  3x1 + 2x2 <= 18" << std::endl;
//      std::cout << "  x1, x2 >= 0" << std::endl;

//     try {
//         int numVarsLP = 2;
//         int numConstraintsLP = 3;

//         Eigen::Matrix<Rational, Eigen::Dynamic, 1> c(numVarsLP);
//         c << Rational(3), Rational(5);

//         Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic> A_lp(numConstraintsLP, numVarsLP);
//         A_lp << Rational(1), Rational(0),  // x1 <= 4
//                Rational(0), Rational(2),  // 2x2 <= 12
//                Rational(3), Rational(2);  // 3x1 + 2x2 <= 18

//         Eigen::Matrix<Rational, Eigen::Dynamic, 1> b_lp(numConstraintsLP);
//         b_lp << Rational(4), Rational(12), Rational(18);

//         SimplexSolver simplex_solver; // Use default options from the namespace class
//         SimplexSolution simplex_result = simplex_solver.solve(c, A_lp, b_lp); // Call solve method
//         printSimplexResult(simplex_result);

//     } catch (const std::invalid_argument& e) {
//         std::cerr << "\nInput Error for Simplex Solver: " << e.what() << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "\nAn unexpected error occurred during simplex solving: " << e.what() << std::endl;
//     }


//     return 0;
// }
#include <iostream>
#include <string>
#include <vector>      // Include vector for variableOrder access
#include <stdexcept>   // Include for exception handling
#include <Eigen/Core>  // Basic Eigen types
#include <Eigen/Dense> // For Matrix/Vector operations if needed beyond printing

// Include your CAS engine headers
#include "lp_parser.hpp"      // Include the parser header
#include "simplex_solver.hpp" // Include the solver header
#include "solution_types.hpp" // Include solution types (defines Rational, SimplexSolution, etc.)

// Use the namespace defined in your headers
using namespace cas_engine;

// Helper function to print Eigen Matrix/Vector of types supporting operator<<
template<typename Derived>
void printRationalMatrix(std::ostream& os, const Eigen::DenseBase<Derived>& mat) {
    // Check if it's likely a row vector for formatting (runtime check)
    bool isRowVector = (mat.rows() == 1 && mat.cols() != 1);
    // Or compile-time check if possible (more complex)
    // bool isRowVector = (Derived::RowsAtCompileTime == 1 && Derived::ColsAtCompileTime != 1);

    os << "["; // Start with bracket
    for(Eigen::Index i = 0; i < mat.rows(); ++i) {
        if (i > 0) os << "; "; // Row separator for multi-row matrices
        for(Eigen::Index j = 0; j < mat.cols(); ++j) {
            os << mat(i, j); // Use the element's operator<< (boost::rational's)
            if (j < mat.cols() - 1) {
                os << (isRowVector ? ", " : "\t"); // Comma for row vector, tab otherwise
            }
        }
    }
     os << "]"; // End with bracket
}


int main() {
    // --- Example Input Strings ---

    // More complex example
    std::string lp_input_complex = R"(
        # Example LP Problem from a textbook
        Maximize:
            50 P + 80 C   # Profit function (P=Product1, C=Product2)
        Subject to:
            # Resource constraints
            P + 2C <= 120       # Resource A
            P + C <= 90         # Resource B
            P <= 70             # Max demand for P
            # Implicit non-negativity P >= 0, C >= 0 is assumed by standard form
            # Explicitly stating them can also be parsed:
            P >= 0
            C >= 0
            # Example of equality
            # P = 50 # Uncomment to test equality
            # Example of fraction/decimal
            0.5 P + 1/3 C <= 40

    )";

    // Simpler example used before
    std::string lp_input_simple = R"(
        Minimize: 10x1 + 5widget_B
        Subject To:
            3x1 + 2widget_B >= 6
            x1 + 4widget_B <= 4
            widget_B > 0.1 # Note: strict inequality treated as >= or <=
            x1 >= 0
            # widget_B >= 0 is implicitly needed for the standard solver form
    )";

    // Choose which input to parse:
    std::string input_to_parse = lp_input_complex; // Change to lp_input_simple to test the other
    std::cout << "==========================================" << std::endl;
    std::cout << "Parsing Input:\n" << input_to_parse << std::endl;
    std::cout << "==========================================" << std::endl;

    // --- Parsing ---
    LPParser parser;
    try {
        ParsedLP parsed_lp = parser.parse(input_to_parse);

        std::cout << "--- Parsing Results ---" << std::endl;
        std::cout << "Objective Type: " << (parsed_lp.isMaximization() ? "Maximize" : "Minimize (standardized to Maximize)") << std::endl;

        std::cout << "Variables (in order): ";
        for(const auto& var : parsed_lp.variableOrder) {
            std::cout << var << " ";
        }
        std::cout << std::endl;

        // Use the helper function for printing Eigen objects with Rationals
        std::cout << "Objective Vector (c): ";
        printRationalMatrix(std::cout, parsed_lp.objectiveCoeffsFinal.transpose()); // Transpose to print as row
        std::cout << std::endl;

        std::cout << "Constraint Matrix (A):\n";
        // Print matrix row by row for clarity if large
        if (parsed_lp.constraintMatrixA.rows() > 0) {
             for (Eigen::Index i = 0; i < parsed_lp.constraintMatrixA.rows(); ++i) {
                 printRationalMatrix(std::cout, parsed_lp.constraintMatrixA.row(i));
                 std::cout << std::endl;
             }
        } else {
            std::cout << "(No constraints)" << std::endl;
        }


        std::cout << "Constraint RHS (b):\n";
        printRationalMatrix(std::cout, parsed_lp.constraintRHS_b);
        std::cout << std::endl;

        // --- Optional: Solve using your SimplexSolver ---
        // Note: The simple solver expects non-negative RHS (b_i >= 0).
        // The parser standardizes >= to <= by negating, so check if any b_i became negative.
        bool can_solve = true;
        if (parsed_lp.constraintRHS_b.size() > 0 && (parsed_lp.constraintRHS_b.array() < 0).any()) {
             std::cerr << "\nWarning: Standardized constraints resulted in negative RHS values. Simple Simplex Solver might fail." << std::endl;
             // can_solve = false; // Optionally prevent solving if strict b>=0 is required by solver
        }

        if (parsed_lp.isValid() && can_solve) {
             std::cout << "\n--- Solving ---" << std::endl;
             SimplexSolver solver; // Use default options
             SimplexSolution solution = solver.solve(
                 parsed_lp.objectiveCoeffsFinal,
                 parsed_lp.constraintMatrixA,
                 parsed_lp.constraintRHS_b
             );

             std::cout << "Solver Status: " << solution.getMessage() << std::endl;

             if(solution.hasOptimalSolution()) {
                 std::cout << "Optimal Value (Z): " << solution.getOptimalValue() << std::endl;
                 std::cout << "Solution Vector (Variables):" << std::endl;
                 // Ensure variableOrder and solution vector sizes match
                 if (static_cast<size_t>(solution.getVariableValues().size()) == parsed_lp.variableOrder.size()) {
                     for(size_t i = 0; i < parsed_lp.variableOrder.size(); ++i) {
                         std::cout << "  " << parsed_lp.variableOrder[i] << " = "
                                   << solution.getVariableValues()(i) << std::endl;
                     }
                 } else {
                      std::cerr << "Warning: Mismatch between number of variables parsed and solution vector size!" << std::endl;
                      std::cout << "Raw solution vector: ";
                      printRationalMatrix(std::cout, solution.getVariableValues().transpose());
                      std::cout << std::endl;
                 }
             } else if (solution.getStatus() == SimplexStatus::UNBOUNDED) {
                 std::cout << "(Objective function is unbounded)" << std::endl;
             } else {
                 // Handle other non-optimal statuses if needed
                 std::cout << "(Solver did not find an optimal solution)" << std::endl;
             }
        } else if (!parsed_lp.isValid()) {
            std::cerr << "\nParsing result is not valid for solving." << std::endl;
        } else {
            std::cout << "\nSkipping solving due to potential issues (e.g., negative RHS)." << std::endl;
        }

    // Catch specific parsing errors
    } catch (const std::runtime_error& e) {
        std::cerr << "\nPARSING ERROR: " << e.what() << std::endl;
        return 1;
    // Catch other standard library errors (e.g., bad_alloc)
    } catch (const std::exception& e) {
         std::cerr << "\nSYSTEM ERROR: " << e.what() << std::endl;
         return 1;
    // Catch any other unknown errors
    } catch (...) {
         std::cerr << "\nAn unknown error occurred!" << std::endl;
         return 1;
    }

    std::cout << "\nExecution finished." << std::endl;
    return 0;
}