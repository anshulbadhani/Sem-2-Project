// lp_parser.hpp
#ifndef LP_PARSER_HPP
#define LP_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <boost/rational.hpp>
#include "solution_types.hpp"
#include <optional>
#include <algorithm> // for std::transform, find_first/last_not_of
#include <cctype>    // for ::isspace, ::tolower
#include <sstream>   // for std::istringstream in split

namespace cas_engine {

enum class ObjectiveType { MAXIMIZE, MINIMIZE, UNSET };

struct ParsedLP {
    ObjectiveType objectiveType = ObjectiveType::UNSET;
    std::map<std::string, Rational> objectiveCoeffsRaw;
    struct Constraint {
        std::map<std::string, Rational> lhsCoeffs;
        std::string relation;
        Rational rhs;
    };
    std::vector<Constraint> constraintsRaw;
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> objectiveCoeffsFinal;
    Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic> constraintMatrixA;
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> constraintRHS_b;
    std::vector<std::string> variableOrder;
    bool isMaximization() const { return objectiveType == ObjectiveType::MAXIMIZE; }
    bool isValid() const { return objectiveType != ObjectiveType::UNSET && !constraintsRaw.empty(); }
};


class LPParser {
public:
    LPParser() = default;
    ParsedLP parse(const std::string& input);

    // --- Define public static utilities inline in the header ---
    inline static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (std::string::npos == first) return ""; // Return empty if all whitespace
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    inline static std::string toLower(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return lowerStr;
    }

    inline static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            std::string trimmed_token = trim(token); // Can call static trim here
            if (!trimmed_token.empty()) tokens.push_back(trimmed_token);
        }
        return tokens;
    }


private:
    // Private members and methods as before
    enum class ParserState { EXPECT_OBJECTIVE, EXPECT_CONSTRAINTS_KEYWORD, EXPECT_CONSTRAINTS };
    ParsedLP m_result;
    ParserState m_currentState = ParserState::EXPECT_OBJECTIVE;
    std::map<std::string, int> m_varIndexMap;
    int m_nextVarIndex = 0;

    void processLine(const std::string& line);
    void parseObjectiveLine(const std::string& line);
    void parseConstraintLine(const std::string& line);
    void parseSingleConstraint(const std::string& constraintStr);
    std::map<std::string, Rational> parseExpression(const std::string& expr);
    std::pair<Rational, std::string> parseTerm(const std::string& termStr);
    void finalizeAndStandardize();
};

// --- Define stringToRational outside the class but within the namespace ---
// It needs access to LPParser::trim, which is now public static and defined above
Rational stringToRational(const std::string& numStrRaw);


} // namespace cas_engine

#endif // LP_PARSER_HPP