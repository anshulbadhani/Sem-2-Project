// lp_parser.cpp
#include "lp_parser.hpp"
#include <sstream>
#include <algorithm> // For sorting in finalize if needed, etc.
#include <cctype>    // For ::isalpha, ::isalnum etc.
#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>
#include <limits>

namespace cas_engine {

// --- String Utilities Definitions REMOVED ---
// The definitions for trim, toLower, split are now inline in the header

// --- Helper Function: Parse String to Rational (Defined outside class) ---
// Needs access to LPParser::trim (which is public static)
Rational stringToRational(const std::string& numStrRaw) {
    std::string numStr = LPParser::trim(numStrRaw); // Call public static method
    if (numStr.empty()) throw std::invalid_argument("Cannot convert empty string to rational.");

    // Check for fraction "num/den"
    size_t slashPos = numStr.find('/');
    if (slashPos != std::string::npos) {
        std::string numPartStr = LPParser::trim(numStr.substr(0, slashPos));
        std::string denPartStr = LPParser::trim(numStr.substr(slashPos + 1));
        if (numPartStr.empty() || denPartStr.empty()) throw std::invalid_argument("Invalid fraction: '" + numStrRaw + "'");
        try {
            long long numerator = std::stoll(numPartStr);
            long long denominator = std::stoll(denPartStr);
            if (denominator == 0) throw std::runtime_error("Den zero in '" + numStrRaw + "'");
            return Rational(numerator, denominator);
        } catch (const std::exception& e) { throw std::runtime_error("Invalid fraction '" + numStrRaw + "': " + e.what()); }
    }

    // Check for decimal "int.frac"
    size_t decimalPos = numStr.find('.');
    if (decimalPos != std::string::npos) {
        std::string intPartStr = (decimalPos == 0 || (decimalPos == 1 && numStr[0] == '-')) ? "0" : numStr.substr(0, decimalPos);
        std::string fracPartStr = numStr.substr(decimalPos + 1);
        if (fracPartStr.empty()) fracPartStr = "0";
        size_t last_not_zero = fracPartStr.find_last_not_of('0');
        if (std::string::npos == last_not_zero) fracPartStr = "0"; else fracPartStr.erase(last_not_zero + 1); if (fracPartStr.empty()) fracPartStr = "0";
        try {
            long long intPart = std::stoll(intPartStr); long long fracPart = 0; long long denominator = 1;
            if(fracPartStr != "0" && !fracPartStr.empty()) {
                fracPart = std::stoll(fracPartStr); int numDecimalPlaces = fracPartStr.length();
                if (numDecimalPlaces > 0) { if (numDecimalPlaces > 18) throw std::out_of_range("Too many decimals");
                    for(int i = 0; i < numDecimalPlaces; ++i) { if (denominator > LLONG_MAX / 10) throw std::out_of_range("Den overflow"); denominator *= 10; }
                }
            }
            long long numerator = 0; bool isNegative = (!intPartStr.empty() && intPartStr[0] == '-');
            if (denominator != 0 && std::abs(intPart) > LLONG_MAX / denominator) throw std::out_of_range("Num overflow (int*den)");
            numerator = intPart * denominator;
            if (isNegative) { if (numerator < LLONG_MIN + fracPart) throw std::out_of_range("Num underflow"); numerator -= fracPart; }
            else { if (numerator > LLONG_MAX - fracPart) throw std::out_of_range("Num overflow"); numerator += fracPart; }
            return Rational(numerator, denominator);
        } catch (const std::exception& e) { throw std::runtime_error("Invalid decimal '" + numStrRaw + "': " + e.what()); }
    }

    // Integer case
    try { return Rational(std::stoll(numStr)); }
    catch (const std::exception& e) { throw std::runtime_error("Invalid integer '" + numStrRaw + "': " + e.what()); }
}


// --- Parsing Logic (Class Methods) ---
// Definitions for parse, processLine, parseObjectiveLine, parseConstraintLine,
// parseSingleConstraint, parseExpression, parseTerm, finalizeAndStandardize
// remain here, but they now call the static LPParser::trim, LPParser::toLower,
// LPParser::split or the free function stringToRational as needed.

ParsedLP LPParser::parse(const std::string& input) {
    m_result = ParsedLP(); m_currentState = ParserState::EXPECT_OBJECTIVE; m_varIndexMap.clear(); m_nextVarIndex = 0;
    std::stringstream ss(input); std::string line;
    // Use static trim method from header
    while (std::getline(ss, line)) processLine(LPParser::trim(line));
    if (m_result.objectiveType == ObjectiveType::UNSET) throw std::runtime_error("Parse Err: Objective missing.");
    if (m_result.constraintsRaw.empty() && m_currentState != ParserState::EXPECT_CONSTRAINTS) if(m_currentState==ParserState::EXPECT_OBJECTIVE || m_currentState==ParserState::EXPECT_CONSTRAINTS_KEYWORD) throw std::runtime_error("Parse Err: Constraints missing/empty.");
    finalizeAndStandardize(); return m_result;
}

void LPParser::processLine(const std::string& line) {
    if (line.empty() || line[0] == '#') return;
    // Use static toLower method from header
    std::string lowerLine = LPParser::toLower(line);
    switch (m_currentState) {
        case ParserState::EXPECT_OBJECTIVE:
            if(lowerLine.rfind("maximize",0)==0||lowerLine.rfind("minimize",0)==0){parseObjectiveLine(line);m_currentState=ParserState::EXPECT_CONSTRAINTS_KEYWORD;}
            else throw std::runtime_error("Parse Err: Expected Max/Min: "+line);
            break;
        case ParserState::EXPECT_CONSTRAINTS_KEYWORD:
            if(lowerLine.rfind("constraints",0)==0||lowerLine.rfind("subject to",0)==0||lowerLine.rfind("s.t.",0)==0||lowerLine.rfind("st",0)==0){
                size_t kE=line.find(':');
                if(kE==std::string::npos){if(lowerLine.rfind("constraints",0)==0) kE=11; else if(lowerLine.rfind("subject to",0)==0) kE=10; else if(lowerLine.rfind("s.t.",0)==0) kE=4; else if(lowerLine.rfind("st",0)==0) kE=2; else kE=0;}
                else kE++;
                // Use static trim
                std::string rem=LPParser::trim(line.substr(kE));
                if(!rem.empty()) parseConstraintLine(rem);
                m_currentState=ParserState::EXPECT_CONSTRAINTS;
            } else {
                parseConstraintLine(line); // Assume start of constraints
                m_currentState=ParserState::EXPECT_CONSTRAINTS;
            }
            break;
        case ParserState::EXPECT_CONSTRAINTS:
            parseConstraintLine(line);
            break;
    }
}

void LPParser::parseObjectiveLine(const std::string& line) {
    // Use static toLower
    std::string lowerLine = LPParser::toLower(line); size_t colonPos = line.find(':'); size_t keywordEnd = 0;
    if(lowerLine.rfind("maximize",0)==0){m_result.objectiveType=ObjectiveType::MAXIMIZE;keywordEnd=8;}else if(lowerLine.rfind("minimize",0)==0){m_result.objectiveType=ObjectiveType::MINIMIZE;keywordEnd=8;}else throw std::runtime_error("Internal Parse Err: Bad objective line.");
    if(colonPos!=std::string::npos&&colonPos>=keywordEnd) keywordEnd=colonPos+1;
    // Use static trim
    std::string expr=LPParser::trim(line.substr(keywordEnd)); if(expr.empty()) throw std::runtime_error("Parse Err: Objective expression missing.");
    m_result.objectiveCoeffsRaw=parseExpression(expr); if(m_result.objectiveType==ObjectiveType::MINIMIZE){for(auto& pair:m_result.objectiveCoeffsRaw) pair.second=-pair.second; m_result.objectiveType=ObjectiveType::MAXIMIZE;}
}

void LPParser::parseConstraintLine(const std::string& line) {
    // Use static split
    std::vector<std::string> constraints = LPParser::split(line, ',');
    for(const std::string& cStr : constraints) if(!cStr.empty()) parseSingleConstraint(cStr);
}

void LPParser::parseSingleConstraint(const std::string& constraintStr) {
    ParsedLP::Constraint constraint; size_t relationPos = std::string::npos; std::string foundRelation; const std::vector<std::string> relations={"<=",">=","<",">","="};
    for(const std::string& rel:relations){relationPos=constraintStr.find(rel);if(relationPos!=std::string::npos){foundRelation=rel; break;}} if(relationPos==std::string::npos) throw std::runtime_error("Parse Err: Relation not found: "+constraintStr);
    // Use static trim
    std::string lhsStr=LPParser::trim(constraintStr.substr(0,relationPos));
    std::string rhsStr=LPParser::trim(constraintStr.substr(relationPos+foundRelation.length()));
    if(lhsStr.empty()) throw std::runtime_error("Parse Err: Missing LHS: "+constraintStr); if(rhsStr.empty()) throw std::runtime_error("Parse Err: Missing RHS: "+constraintStr);
    constraint.lhsCoeffs=parseExpression(lhsStr);
    // Use free function stringToRational
    try { constraint.rhs = stringToRational(rhsStr); } catch(const std::exception& e){ throw std::runtime_error("Parse Err: Cannot parse RHS '"+rhsStr+"': "+e.what());}
    constraint.relation=foundRelation; m_result.constraintsRaw.push_back(constraint);
}

std::map<std::string, Rational> LPParser::parseExpression(const std::string& expr) {
    std::map<std::string,Rational> coeffs; std::string currentTerm; char sign='+';
    for(size_t i=0; i<expr.length(); ++i){
        char c=expr[i]; if(isspace(c)) continue;
        if((c=='+'||c=='-') && !currentTerm.empty()){ auto termResult=parseTerm(sign+currentTerm); if(termResult.second.empty()) throw std::runtime_error("Parse Err: Constant term '"+currentTerm+"' in expr: "+expr); coeffs[termResult.second]+=termResult.first; currentTerm.clear(); sign=c; }
        else if (c == '+' && currentTerm.empty()) sign = '+'; else if (c == '-' && currentTerm.empty()) sign = '-'; else currentTerm+=c;
    }
    if(!currentTerm.empty()){auto termResult=parseTerm(sign+currentTerm); if(termResult.second.empty()) throw std::runtime_error("Parse Err: Constant term '"+currentTerm+"' at end of expr: "+expr); coeffs[termResult.second]+=termResult.first;}
    return coeffs;
}

std::pair<Rational, std::string> LPParser::parseTerm(const std::string& termStrRaw) {
    // Use static trim
    std::string termStr=LPParser::trim(termStrRaw); if(termStr.empty()) return {Rational(0),""};
    Rational coefficient(1); std::string varName=""; size_t firstLetterPos=std::string::npos;
    for(size_t i=0; i<termStr.length(); ++i) if(std::isalpha(termStr[i])){firstLetterPos=i; break;}
    std::string coeffStr;
    if(firstLetterPos==std::string::npos){ coeffStr=termStr; try{return {stringToRational(coeffStr),""};} catch(const std::exception& e){throw std::runtime_error("Parse Err: Invalid constant term '"+termStrRaw+"': "+e.what());}}
    else {
        // Use static trim
        coeffStr=LPParser::trim(termStr.substr(0,firstLetterPos));
        varName=LPParser::trim(termStr.substr(firstLetterPos));
        if(varName.empty()||!std::isalpha(varName[0])) throw std::runtime_error("Invalid var name from term '"+termStrRaw+"'"); for(char vc:varName) if(!std::isalnum(vc)) throw std::runtime_error("Invalid char in var name '"+varName+"'");
        if(coeffStr.empty()||coeffStr=="+") coefficient=1; else if(coeffStr=="-") coefficient=-1; else {try{coefficient=stringToRational(coeffStr);} catch(const std::exception& e){throw std::runtime_error("Parse Err: Cannot parse coeff '"+coeffStr+"': "+e.what());}}
        if(m_varIndexMap.find(varName)==m_varIndexMap.end()) m_varIndexMap[varName]=m_nextVarIndex++;
    }
    return {coefficient,varName};
}

void LPParser::finalizeAndStandardize() {
    int numVars=m_nextVarIndex; m_result.variableOrder.resize(numVars); for(const auto& pair:m_varIndexMap) m_result.variableOrder[pair.second]=pair.first;
    std::vector<ParsedLP::Constraint> standardizedConstraints;
    for(const auto& rawC:m_result.constraintsRaw){ /* ... Standardize constraints ... */ if(rawC.relation=="<="||rawC.relation=="<") standardizedConstraints.push_back(rawC); else if(rawC.relation==">="||rawC.relation==">"){ParsedLP::Constraint stdC; stdC.relation="<="; stdC.rhs=-rawC.rhs; for(const auto& p:rawC.lhsCoeffs) stdC.lhsCoeffs[p.first]=-p.second; standardizedConstraints.push_back(stdC);} else if(rawC.relation=="="){ParsedLP::Constraint leC=rawC; leC.relation="<="; standardizedConstraints.push_back(leC); ParsedLP::Constraint geC; geC.relation="<="; geC.rhs=-rawC.rhs; for(const auto& p:rawC.lhsCoeffs) geC.lhsCoeffs[p.first]=-p.second; standardizedConstraints.push_back(geC);} else throw std::runtime_error("Internal Parse Err: Unknown relation");}
    int numStdCons=standardizedConstraints.size(); if(numStdCons==0&&numVars>0) std::cerr<<"Warning: LP objective but no constraints."<<std::endl;
    m_result.constraintMatrixA=Eigen::Matrix<Rational,Eigen::Dynamic,Eigen::Dynamic>::Zero(numStdCons,numVars); m_result.constraintRHS_b=Eigen::Matrix<Rational,Eigen::Dynamic,1>::Zero(numStdCons); m_result.objectiveCoeffsFinal=Eigen::Matrix<Rational,Eigen::Dynamic,1>::Zero(numVars);
    for(int i=0; i<numStdCons; ++i){ const auto& constraint=standardizedConstraints[i]; m_result.constraintRHS_b(i)=constraint.rhs; for(const auto& pair:constraint.lhsCoeffs){if(m_varIndexMap.count(pair.first)) m_result.constraintMatrixA(i,m_varIndexMap[pair.first])=pair.second; else throw std::runtime_error("Internal Parse Err: Var '"+pair.first+"' missing");}}
    for(const auto& pair:m_result.objectiveCoeffsRaw){ if(m_varIndexMap.count(pair.first)) m_result.objectiveCoeffsFinal(m_varIndexMap[pair.first])=pair.second; else throw std::runtime_error("Internal Parse Err: Var '"+pair.first+"' missing");}
}


} // namespace cas_engine