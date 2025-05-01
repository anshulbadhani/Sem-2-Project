#include <SFML/Graphics.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>         // Required for solving 2x2 systems
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>          // For std::setprecision
#include <cmath>            // For std::abs, std::max, std::min, std::sqrt, std::atan2, std::ceil, std::floor
#include <sstream>          // For std::stringstream
#include <boost/rational.hpp>
#include <boost/lexical_cast.hpp> // Include for lexical_cast
#include <stdexcept>              // Include for std::logic_error, std::exception
#include <limits>                 // For numeric_limits
#include <optional>             // Include for std::optional
#include <algorithm>            // Include for std::sort, std::max, std::min
#include <numeric>              // Include for std::accumulate

// Include your CAS engine headers
#include "solution_types.hpp"
#include "simplex_solver.hpp"

// Use your engine's namespace
using namespace cas_engine;

// --- Configuration ---
const unsigned int WINDOW_WIDTH = 900;
const unsigned int WINDOW_HEIGHT = 800;
const float PADDING = 60.0f;
const double DEFAULT_VIEW_RANGE = 10.0;
const double BOUND_PADDING_FACTOR = 1.15;
const double FEASIBILITY_TOLERANCE = 1e-6;
const double GRID_SPACING = 1.0; // Spacing for grid lines


// --- Coordinate Transformation & Math Helpers ---

// Global math bounds (determined after calculating vertices)
double MATH_X_MIN = -1.0;
double MATH_X_MAX = DEFAULT_VIEW_RANGE;
double MATH_Y_MIN = -1.0;
double MATH_Y_MAX = DEFAULT_VIEW_RANGE;

// Convert mathematical coordinates to SFML screen coordinates
sf::Vector2f mathToScreen(double mathX, double mathY) {
    double graphWidth = WINDOW_WIDTH - 2 * PADDING;
    double graphHeight = WINDOW_HEIGHT - 2 * PADDING;
    double mathWidth = MATH_X_MAX - MATH_X_MIN;
    double mathHeight = MATH_Y_MAX - MATH_Y_MIN;

    if (std::abs(mathWidth) < 1e-9 || std::abs(mathHeight) < 1e-9) {
        return sf::Vector2f(PADDING + graphWidth / 2.0f, WINDOW_HEIGHT - PADDING - graphHeight / 2.0f);
    }
    float screenX = PADDING + static_cast<float>((mathX - MATH_X_MIN) / mathWidth * graphWidth);
    float screenY = (WINDOW_HEIGHT - PADDING) - static_cast<float>((mathY - MATH_Y_MIN) / mathHeight * graphHeight);
    return sf::Vector2f(screenX, screenY);
}

// Helper to calculate squared length of a vector
float vectorLengthSquared(const sf::Vector2f& v) {
    return v.x * v.x + v.y * v.y;
}

// Helper function to solve a 2x2 linear system M*x = v
std::optional<Eigen::Vector2d> solve2x2System(
    const Eigen::Matrix<double, 2, 2>& M,
    const Eigen::Vector2d& v)
{
    Eigen::FullPivLU<Eigen::Matrix<double, 2, 2>> lu(M);
    if (!lu.isInvertible()) {
        return std::nullopt;
    }
    Eigen::Vector2d solution = lu.solve(v);
    if (!solution.allFinite()) {
         return std::nullopt;
    }
    return solution;
}

// Helper to format constraint string (Ax <= b)
std::string formatConstraintString(const Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic>& A,
                                  const Eigen::Matrix<Rational, Eigen::Dynamic, 1>& b,
                                  int constraintIndex)
{
    std::stringstream ss;
    bool firstTerm = true;
    Rational a1 = A(constraintIndex, 0);
    if (a1 != 0) {
        if (a1 == -1) ss << "-";
        else if (a1 != 1) try { ss << boost::lexical_cast<std::string>(a1); } catch (...) { ss << "?"; }
        ss << "x₁"; // Using subscript 1
        firstTerm = false;
    }
    Rational a2 = A(constraintIndex, 1);
    if (a2 != 0) {
        if (!firstTerm) ss << (a2 > 0 ? " + " : " - ");
        else if (a2 < 0) ss << "-";
        Rational abs_a2 = boost::abs(a2);
        if (abs_a2 != 1) try { ss << boost::lexical_cast<std::string>(abs_a2); } catch (...) { ss << "?"; }
        ss << "x₂"; // Using subscript 2
    }
    // Handle cases where both a1 and a2 are zero (e.g., 0 <= 5)
    if (firstTerm && a2 == 0) {
         ss << "0";
    }
    ss << " ≤ "; // Using less than or equal symbol
    try { ss << boost::lexical_cast<std::string>(b(constraintIndex)); } catch (...) { ss << "?"; }
    return ss.str();
}


// --- Main Application ---
int main() {
    // --- 1. Define the LP Problem ---
    // Example: Maximize Z = 3x1 + 5x2
    int numVarsLP = 2;
    int numConstraintsLP = 3;
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> c(numVarsLP);
    c << Rational(3), Rational(5);
    Eigen::Matrix<Rational, Eigen::Dynamic, Eigen::Dynamic> A_lp(numConstraintsLP, numVarsLP);
    A_lp << Rational(1), Rational(0), // x1 <= 4
           Rational(0), Rational(2), // 2x2 <= 12 -> x2 <= 6
           Rational(3), Rational(2); // 3x1 + 2x2 <= 18
    Eigen::Matrix<Rational, Eigen::Dynamic, 1> b_lp(numConstraintsLP);
    b_lp << Rational(4), Rational(12), Rational(18);

     // Convert constraints to double for intersection calculations
     Eigen::MatrixXd A_double(numConstraintsLP, numVarsLP);
     Eigen::VectorXd b_double(numConstraintsLP);
     try {
         for(int i=0; i<numConstraintsLP; ++i) {
             b_double(i) = boost::rational_cast<double>(b_lp(i));
             for(int j=0; j<numVarsLP; ++j) {
                 A_double(i,j) = boost::rational_cast<double>(A_lp(i,j));
             }
         }
     } catch (const std::exception& e) {
         std::cerr << "Error converting rationals to double for geometry: " << e.what() << std::endl;
         return 1; // Cannot proceed with calculations
     }

    // --- 2. Solve the LP ---
    SimplexSolver simplex_solver;
    SimplexSolution solution;
    bool solved = false;
    try {
        solution = simplex_solver.solve(c, A_lp, b_lp);
        solved = true;
        std::cout << "Simplex solver finished with status: " << solution.getMessage() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error solving LP: " << e.what() << std::endl;
    }

    // --- 3. Calculate Potential Vertices (including view boundary intersections) ---
    std::vector<Eigen::Vector2d> potentialVertices;
    double tempMaxX = 1.0, tempMaxY = 1.0; // Initial guess for max coords

    // 3a. Intersections of constraints with axes (x1=0, x2=0)
    potentialVertices.push_back({0.0, 0.0}); // Origin
    for (int i = 0; i < numConstraintsLP; ++i) {
        double a1 = A_double(i, 0), a2 = A_double(i, 1), b_val = b_double(i);
        if (std::abs(a1) > FEASIBILITY_TOLERANCE) { // x1-axis intercept (y=0)
            double x_int = b_val / a1; potentialVertices.push_back({x_int, 0.0}); tempMaxX = std::max(tempMaxX, x_int);
        }
        if (std::abs(a2) > FEASIBILITY_TOLERANCE) { // x2-axis intercept (x=0)
            double y_int = b_val / a2; potentialVertices.push_back({0.0, y_int}); tempMaxY = std::max(tempMaxY, y_int);
        }
    }

    // 3b. Intersections of constraints with each other
    if (numConstraintsLP >= 2) {
        for (int i = 0; i < numConstraintsLP; ++i) {
            for (int j = i + 1; j < numConstraintsLP; ++j) {
                 Eigen::Matrix<double, 2, 2> M; M << A_double(i, 0), A_double(i, 1), A_double(j, 0), A_double(j, 1);
                 Eigen::Vector2d v = {b_double(i), b_double(j)};
                 std::optional<Eigen::Vector2d> intersection = solve2x2System(M, v);
                 if (intersection.has_value()) {
                     potentialVertices.push_back(*intersection);
                     tempMaxX = std::max(tempMaxX, intersection->x());
                     tempMaxY = std::max(tempMaxY, intersection->y());
                 }
            }
        }
    }

    // 3c. Set initial bounds based on intercepts/intersections found so far
    MATH_X_MIN = -0.1 * std::max(tempMaxX, DEFAULT_VIEW_RANGE);
    MATH_Y_MIN = -0.1 * std::max(tempMaxY, DEFAULT_VIEW_RANGE);
    MATH_X_MAX = std::max({tempMaxX, DEFAULT_VIEW_RANGE, 1.0}) * BOUND_PADDING_FACTOR;
    MATH_Y_MAX = std::max({tempMaxY, DEFAULT_VIEW_RANGE, 1.0}) * BOUND_PADDING_FACTOR;
    if(MATH_X_MAX <= MATH_X_MIN + 1e-6) MATH_X_MAX = MATH_X_MIN + DEFAULT_VIEW_RANGE;
    if(MATH_Y_MAX <= MATH_Y_MIN + 1e-6) MATH_Y_MAX = MATH_Y_MIN + DEFAULT_VIEW_RANGE;

    // 3d. Add view boundary corners (potential vertices if region fills the view)
    potentialVertices.push_back({MATH_X_MIN, MATH_Y_MIN});
    potentialVertices.push_back({MATH_X_MAX, MATH_Y_MIN});
    potentialVertices.push_back({MATH_X_MAX, MATH_Y_MAX});
    potentialVertices.push_back({MATH_X_MIN, MATH_Y_MAX});

    // 3e. Add intersections of constraints WITH view boundaries
     for (int i = 0; i < numConstraintsLP; ++i) {
        double a1 = A_double(i, 0), a2 = A_double(i, 1), b_val = b_double(i);
         if (std::abs(a2) > FEASIBILITY_TOLERANCE) { double y_at_xmin = (b_val - a1 * MATH_X_MIN) / a2; if (y_at_xmin >= MATH_Y_MIN && y_at_xmin <= MATH_Y_MAX) potentialVertices.push_back({MATH_X_MIN, y_at_xmin}); }
         if (std::abs(a2) > FEASIBILITY_TOLERANCE) { double y_at_xmax = (b_val - a1 * MATH_X_MAX) / a2; if (y_at_xmax >= MATH_Y_MIN && y_at_xmax <= MATH_Y_MAX) potentialVertices.push_back({MATH_X_MAX, y_at_xmax}); }
         if (std::abs(a1) > FEASIBILITY_TOLERANCE) { double x_at_ymin = (b_val - a2 * MATH_Y_MIN) / a1; if (x_at_ymin >= MATH_X_MIN && x_at_ymin <= MATH_X_MAX) potentialVertices.push_back({x_at_ymin, MATH_Y_MIN}); }
         if (std::abs(a1) > FEASIBILITY_TOLERANCE) { double x_at_ymax = (b_val - a2 * MATH_Y_MAX) / a1; if (x_at_ymax >= MATH_X_MIN && x_at_ymax <= MATH_X_MAX) potentialVertices.push_back({x_at_ymax, MATH_Y_MAX}); }
     }


    // --- 4. Filter Feasible Vertices and Sort for Polygon ---
    std::vector<Eigen::Vector2d> feasibleVertices;
    for (const auto& p : potentialVertices) {
        bool feasible = true;
        if (p.x() < MATH_X_MIN - FEASIBILITY_TOLERANCE || p.x() > MATH_X_MAX + FEASIBILITY_TOLERANCE ||
            p.y() < MATH_Y_MIN - FEASIBILITY_TOLERANCE || p.y() > MATH_Y_MAX + FEASIBILITY_TOLERANCE) feasible = false;
        else if (p.x() < -FEASIBILITY_TOLERANCE || p.y() < -FEASIBILITY_TOLERANCE) feasible = false;
        else for (int k = 0; k < numConstraintsLP; ++k) if (A_double(k, 0) * p.x() + A_double(k, 1) * p.y() > b_double(k) + FEASIBILITY_TOLERANCE) { feasible = false; break; }

        if (feasible) {
             bool duplicate = false;
             for(const auto& fv : feasibleVertices) if ((p - fv).squaredNorm() < FEASIBILITY_TOLERANCE * 10) { duplicate = true; break; }
             if (!duplicate) feasibleVertices.push_back(p);
        }
    }

    // Sort feasible vertices by angle around centroid for polygon drawing
    if (feasibleVertices.size() >= 3) {
        Eigen::Vector2d centroid = {0.0, 0.0};
        // Safer centroid calculation: handle potential for empty vector
        if (!feasibleVertices.empty()) {
            for(const auto& v : feasibleVertices) { centroid += v; }
            centroid /= static_cast<double>(feasibleVertices.size());
        }

        // Sort by angle using the calculated centroid
        std::sort(feasibleVertices.begin(), feasibleVertices.end(),
                  [&centroid](const Eigen::Vector2d& a, const Eigen::Vector2d& b) { // *** Correct capture [&centroid] ***
            double angleA = std::atan2(a.y() - centroid.y(), a.x() - centroid.x());
            double angleB = std::atan2(b.y() - centroid.y(), b.x() - centroid.x());
            return angleA < angleB;
        });
    }
    


    // --- 5. Define Simplex Path (Still Manual Example) ---
    std::vector<Eigen::Vector2d> simplexPathManual;
    if (solved && solution.hasOptimalSolution()) {
         simplexPathManual.push_back({0.0, 0.0});
         simplexPathManual.push_back({0.0, 6.0}); // Values from example
         try {
             simplexPathManual.push_back({ boost::rational_cast<double>(solution.getVariableValues()(0)),
                                           boost::rational_cast<double>(solution.getVariableValues()(1)) });
         } catch (const std::exception& e) {
              std::cerr << "Error getting optimal values for manual path: " << e.what() << std::endl;
         }
    }

    // --- 6. Setup SFML Window and Font ---
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Simplex Visualization (2D)");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/DejaVuSans.ttf")) {
         if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
             std::cerr << "Error loading font!" << std::endl; return 1;
         }
    }

    // --- 7. Prepare SFML Drawables ---
    sf::Color axisColor(180, 180, 180);
    sf::Color constraintColor = sf::Color::Cyan;
    sf::Color pathColor = sf::Color::Red;
    sf::Color solutionColor = sf::Color::Green;
    sf::Color vertexColor(255, 165, 0);
    sf::Color labelColor = sf::Color::White;
    sf::Color gridColor(80, 80, 80);
    sf::Color feasibleRegionColor(0, 80, 0, 90);
    unsigned int labelSize = 12;

    // Grid Lines
    std::vector<sf::VertexArray> gridLines;
    // Vertical lines
    for (double x = std::ceil(MATH_X_MIN / GRID_SPACING) * GRID_SPACING; x <= MATH_X_MAX; x += GRID_SPACING) {
        if (std::abs(x) < FEASIBILITY_TOLERANCE / 10.0) continue;
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = mathToScreen(x, MATH_Y_MIN); line[0].color = gridColor;
        line[1].position = mathToScreen(x, MATH_Y_MAX); line[1].color = gridColor;
        gridLines.push_back(line);
    }
    // Horizontal lines
    for (double y = std::ceil(MATH_Y_MIN / GRID_SPACING) * GRID_SPACING; y <= MATH_Y_MAX; y += GRID_SPACING) {
        if (std::abs(y) < FEASIBILITY_TOLERANCE / 10.0) continue;
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = mathToScreen(MATH_X_MIN, y); line[0].color = gridColor;
        line[1].position = mathToScreen(MATH_X_MAX, y); line[1].color = gridColor;
        gridLines.push_back(line);
    }

    // Feasible Region Polygon
    sf::ConvexShape feasiblePolygon;
    if (feasibleVertices.size() >= 3) {
        feasiblePolygon.setPointCount(feasibleVertices.size());
        for (size_t i = 0; i < feasibleVertices.size(); ++i) {
            feasiblePolygon.setPoint(i, mathToScreen(feasibleVertices[i].x(), feasibleVertices[i].y()));
        }
        feasiblePolygon.setFillColor(feasibleRegionColor);
    } else {
         std::cout << "Note: Fewer than 3 feasible vertices found. Cannot draw feasible region polygon." << std::endl;
    }

    // Axes & Labels
    sf::VertexArray axes(sf::Lines, 4);
    axes[0].position=mathToScreen(MATH_X_MIN,0); axes[0].color=axisColor; axes[1].position=mathToScreen(MATH_X_MAX,0); axes[1].color=axisColor;
    axes[2].position=mathToScreen(0,MATH_Y_MIN); axes[2].color=axisColor; axes[3].position=mathToScreen(0,MATH_Y_MAX); axes[3].color=axisColor;
    sf::Text xLabel("x₁",font,labelSize); xLabel.setFillColor(labelColor); xLabel.setPosition(mathToScreen(MATH_X_MAX,0.0)+sf::Vector2f(-15,5));
    sf::Text yLabel("x₂",font,labelSize); yLabel.setFillColor(labelColor); yLabel.setPosition(mathToScreen(0.0,MATH_Y_MAX)+sf::Vector2f(5,-15));
    sf::Text originLabel("0",font,labelSize); originLabel.setFillColor(labelColor); originLabel.setOrigin(originLabel.getLocalBounds().width/2.f,0); originLabel.setPosition(mathToScreen(0.0,0.0)+sf::Vector2f(0,5));

    // Constraint Lines and Labels
    std::vector<sf::VertexArray> constraintLines; std::vector<sf::Text> constraintLabels;
    for(int i=0; i<numConstraintsLP; ++i){
        double a1=A_double(i,0), a2=A_double(i,1), b_val=b_double(i); sf::VertexArray line(sf::Lines,2); std::vector<sf::Vector2f> points;
        if(std::abs(a2)>1e-9){double y_int=b_val/a2;if(y_int>=MATH_Y_MIN&&y_int<=MATH_Y_MAX) points.push_back(mathToScreen(0.0,y_int));} if(std::abs(a1)>1e-9){double x_int=b_val/a1;if(x_int>=MATH_X_MIN&&x_int<=MATH_X_MAX) points.push_back(mathToScreen(x_int,0.0));}
        if(points.size()<2&&std::abs(a2)>1e-9){double y_at_xmax=(b_val-a1*MATH_X_MAX)/a2;if(y_at_xmax>=MATH_Y_MIN&&y_at_xmax<=MATH_Y_MAX) points.push_back(mathToScreen(MATH_X_MAX,y_at_xmax));} if(points.size()<2&&std::abs(a1)>1e-9){double x_at_ymax=(b_val-a2*MATH_Y_MAX)/a1;if(x_at_ymax>=MATH_X_MIN&&x_at_ymax<=MATH_X_MAX) points.push_back(mathToScreen(x_at_ymax,MATH_Y_MAX));}
        if(points.size()<2&&std::abs(a2)>1e-9&&MATH_X_MIN<-1e-9){double y_at_xmin=(b_val-a1*MATH_X_MIN)/a2;if(y_at_xmin>=MATH_Y_MIN&&y_at_xmin<=MATH_Y_MAX) points.push_back(mathToScreen(MATH_X_MIN,y_at_xmin));} if(points.size()<2&&std::abs(a1)>1e-9&&MATH_Y_MIN<-1e-9){double x_at_ymin=(b_val-a2*MATH_Y_MIN)/a1;if(x_at_ymin>=MATH_X_MIN&&x_at_ymin<=MATH_X_MAX) points.push_back(mathToScreen(x_at_ymin,MATH_Y_MIN));}
        if(points.size()>=2){ const float min_pixel_dist_sq=1.0f; sf::Vector2f p1,p2; bool foundPair=false; if(vectorLengthSquared(points[0]-points[1])>min_pixel_dist_sq){p1=points[0]; p2=points[1]; foundPair=true;} else if(points.size()>=3&&vectorLengthSquared(points[0]-points[2])>min_pixel_dist_sq){p1=points[0]; p2=points[2]; foundPair=true;} else if(points.size()>=3&&vectorLengthSquared(points[1]-points[2])>min_pixel_dist_sq){p1=points[1]; p2=points[2]; foundPair=true;}
            if(foundPair){ line[0].position=p1; line[0].color=constraintColor; line[1].position=p2; line[1].color=constraintColor; constraintLines.push_back(line);
                sf::Text label; try{label.setString(formatConstraintString(A_lp,b_lp,i));}catch(...){label.setString("Err");} label.setFont(font); label.setCharacterSize(labelSize-1); label.setFillColor(constraintColor);
                sf::Vector2f midPoint=(p1+p2)/2.0f; sf::Vector2f direction=p2-p1; sf::Vector2f perp(-direction.y,direction.x); float perpL=std::sqrt(vectorLengthSquared(perp)); if(perpL>1e-6) perp/=perpL; float offsetAmount=8.0f; sf::Vector2f labelOffset=perp*offsetAmount; if((labelOffset.x*a1+labelOffset.y*a2)<0) labelOffset=-labelOffset; if(std::abs(direction.x)<1.f&&midPoint.x<mathToScreen(0,0).x+20) labelOffset.x=std::abs(labelOffset.x); if(std::abs(direction.y)<1.f&&midPoint.y>mathToScreen(0,0).y-20) labelOffset.y=-std::abs(labelOffset.y);
                label.setOrigin(label.getLocalBounds().width/2.f, label.getLocalBounds().height); label.setPosition(midPoint+labelOffset); constraintLabels.push_back(label);
            }
        }else{ std::cerr << "Warning: Could not find two points for constraint " << i+1 << std::endl; }
    }

    // Simplex Path Drawables
    std::vector<sf::VertexArray> pathLines; std::vector<sf::CircleShape> pathVertices; float vertexRadius = 5.0f;
    if(simplexPathManual.size()>1){for(size_t i=0; i<simplexPathManual.size()-1; ++i){sf::VertexArray pathLine(sf::Lines,2);pathLine[0].position=mathToScreen(simplexPathManual[i].x(),simplexPathManual[i].y()); pathLine[0].color=pathColor; pathLine[1].position=mathToScreen(simplexPathManual[i+1].x(),simplexPathManual[i+1].y()); pathLine[1].color=pathColor; pathLines.push_back(pathLine);}}
    for(const auto& vertex : simplexPathManual){sf::CircleShape node(vertexRadius); node.setFillColor(vertexColor); node.setOrigin(vertexRadius,vertexRadius); node.setPosition(mathToScreen(vertex.x(),vertex.y())); pathVertices.push_back(node);}

    // Optimal Solution Marker & Label
    sf::CircleShape optimalMarker(vertexRadius*1.5f); optimalMarker.setFillColor(solutionColor); optimalMarker.setOutlineColor(sf::Color::White); optimalMarker.setOutlineThickness(1.0f); optimalMarker.setOrigin(optimalMarker.getRadius(),optimalMarker.getRadius());
    bool optimalExists=false; Eigen::Vector2d optimalPointCoords;
    if(solved&&solution.hasOptimalSolution()){try{optimalPointCoords.x()=boost::rational_cast<double>(solution.getVariableValues()(0)); optimalPointCoords.y()=boost::rational_cast<double>(solution.getVariableValues()(1)); optimalMarker.setPosition(mathToScreen(optimalPointCoords.x(),optimalPointCoords.y())); optimalExists=true;} catch(...) {}}
    sf::Text optimalLabelText; optimalLabelText.setFont(font); optimalLabelText.setCharacterSize(labelSize); optimalLabelText.setFillColor(solutionColor);
    if(optimalExists){ std::stringstream ssOpt; ssOpt << std::fixed << std::setprecision(2) << "(" << optimalPointCoords.x() << ", " << optimalPointCoords.y() << ")"; optimalLabelText.setString(ssOpt.str()); optimalLabelText.setOrigin(0, optimalLabelText.getLocalBounds().height + optimalLabelText.getLocalBounds().top); optimalLabelText.setPosition(optimalMarker.getPosition() + sf::Vector2f(optimalMarker.getRadius()+2, optimalMarker.getRadius()+2)); }

    // Status Text
    sf::Text infoText; infoText.setFont(font); infoText.setCharacterSize(14); infoText.setFillColor(sf::Color::White); infoText.setPosition(PADDING/2, PADDING/4);


    // --- 8. Main Loop ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // Update Status Text
         std::stringstream ss; if (solved) { ss << solution.getMessage(); if (solution.hasOptimalSolution()) try { ss << "\nZ = " << solution.getOptimalValue(); } catch (...) {} } else { ss << "Solver failed or not run."; } infoText.setString(ss.str());

        // --- Drawing --- Order Matters! ---
        window.clear(sf::Color(40, 40, 45));
        for(const auto& line : gridLines) window.draw(line);
        if (feasiblePolygon.getPointCount() >= 3) window.draw(feasiblePolygon);
        window.draw(axes); window.draw(xLabel); window.draw(yLabel); window.draw(originLabel);
        for (const auto& line : constraintLines) window.draw(line);
        for (const auto& label : constraintLabels) window.draw(label);
        for (const auto& line : pathLines) window.draw(line);
        for (const auto& vertex : pathVertices) window.draw(vertex);
        if(optimalExists) { window.draw(optimalMarker); window.draw(optimalLabelText); }
        window.draw(infoText);
        window.display();
    }

    return 0;
}