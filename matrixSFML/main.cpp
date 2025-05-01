
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include "matrix.hpp"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int GRID_SIZE_DEFAULT = 40;
const sf::Vector2f ORIGIN(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

// UI Layout constants
const int LEFT_PANEL_WIDTH = 250;
const int RIGHT_PANEL_WIDTH = 300;
const int GRID_PANEL_WIDTH = WINDOW_WIDTH - LEFT_PANEL_WIDTH - RIGHT_PANEL_WIDTH;
const sf::Vector2f GRID_ORIGIN_DEFAULT(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH / 2, WINDOW_HEIGHT / 2);

// Zoom and pan variables
float gridZoom = 0.75f; // Start at 75% (25% zoomed out)
int gridSize = static_cast<int>(GRID_SIZE_DEFAULT * gridZoom);
sf::Vector2f gridOrigin = GRID_ORIGIN_DEFAULT;
sf::Vector2f gridPan(0, 0);

// Function to round a float to 2 decimal places and convert to string
std::string roundToString(float value) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    return ss.str();
}

sf::Vector2f toSFMLCoords(const sf::Vector2f& vec) {
    return sf::Vector2f(
        gridOrigin.x + gridPan.x + vec.x * gridSize,
        gridOrigin.y + gridPan.y - vec.y * gridSize
    );
}

// Function to check if a point is within the grid panel
bool isInGridPanel(const sf::Vector2f& point) {
    return point.x >= LEFT_PANEL_WIDTH &&
           point.x <= LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH &&
           point.y >= 0 &&
           point.y <= WINDOW_HEIGHT;
}

void drawGrid(sf::RenderWindow& window, sf::Font& font) {
    // Draw panel dividers
    sf::RectangleShape leftPanel(sf::Vector2f(LEFT_PANEL_WIDTH, WINDOW_HEIGHT));
    leftPanel.setFillColor(sf::Color(30, 30, 30));
    window.draw(leftPanel);
   
    sf::RectangleShape rightPanel(sf::Vector2f(RIGHT_PANEL_WIDTH, WINDOW_HEIGHT));
    rightPanel.setPosition(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH, 0);
    rightPanel.setFillColor(sf::Color(30, 30, 30));
    window.draw(rightPanel);
   
    // Create a view for the grid panel to clip content
    sf::View gridView;
    gridView.reset(sf::FloatRect(LEFT_PANEL_WIDTH, 0, GRID_PANEL_WIDTH, WINDOW_HEIGHT));
    gridView.setViewport(sf::FloatRect(
        LEFT_PANEL_WIDTH / static_cast<float>(WINDOW_WIDTH),
        0,
        GRID_PANEL_WIDTH / static_cast<float>(WINDOW_WIDTH),
        1.0f
    ));
   
    // Save the original view
    sf::View originalView = window.getView();
   
    // Set the grid view for drawing grid elements
    window.setView(gridView);
   
    // Draw grid in center panel
    int gridExtent = std::max(GRID_PANEL_WIDTH, WINDOW_HEIGHT) / gridSize + 2;
   
    // Calculate grid start positions
    int startX = static_cast<int>((LEFT_PANEL_WIDTH - gridOrigin.x - gridPan.x) / gridSize) - 1;
    int startY = static_cast<int>((gridOrigin.y + gridPan.y) / gridSize) - 1;
   
    // Draw vertical grid lines
    for (int i = 0; i <= gridExtent * 2; i++) {
        int x = startX + i;
        float screenX = gridOrigin.x + gridPan.x + x * gridSize;
       
        if (screenX >= LEFT_PANEL_WIDTH && screenX <= LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(screenX, 0),
                          (x == 0) ? sf::Color::Green : sf::Color(80, 80, 80)),
                sf::Vertex(sf::Vector2f(screenX, WINDOW_HEIGHT),
                          (x == 0) ? sf::Color::Green : sf::Color(80, 80, 80))
            };
            window.draw(line, 2, sf::Lines);
        }
    }

    // Draw horizontal grid lines
    for (int i = 0; i <= gridExtent * 2; i++) {
        int y = startY - i;
        float screenY = gridOrigin.y + gridPan.y - y * gridSize;
       
        if (screenY >= 0 && screenY <= WINDOW_HEIGHT) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(LEFT_PANEL_WIDTH, screenY),
                          (y == 0) ? sf::Color::Red : sf::Color(80, 80, 80)),
                sf::Vertex(sf::Vector2f(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH, screenY),
                          (y == 0) ? sf::Color::Red : sf::Color(80, 80, 80))
            };
            window.draw(line, 2, sf::Lines);
        }
    }
   
    // Draw X-axis
    sf::Vertex xAxis[] = {
        sf::Vertex(sf::Vector2f(LEFT_PANEL_WIDTH, gridOrigin.y + gridPan.y), sf::Color::Green),
        sf::Vertex(sf::Vector2f(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH, gridOrigin.y + gridPan.y), sf::Color::Green)
    };
    window.draw(xAxis, 2, sf::Lines);
   
    // Draw Y-axis
    sf::Vertex yAxis[] = {
        sf::Vertex(sf::Vector2f(gridOrigin.x + gridPan.x, 0), sf::Color::Red),
        sf::Vertex(sf::Vector2f(gridOrigin.x + gridPan.x, WINDOW_HEIGHT), sf::Color::Red)
    };
    window.draw(yAxis, 2, sf::Lines);
   
    // Draw axis labels
    sf::Text xLabel("X", font, 16);
    xLabel.setFillColor(sf::Color::Green);
    sf::Vector2f xLabelPos(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH - 30, gridOrigin.y + gridPan.y + 10);
    if (isInGridPanel(xLabelPos)) {
        xLabel.setPosition(xLabelPos);
        window.draw(xLabel);
    }
   
    sf::Text yLabel("Y", font, 16);
    yLabel.setFillColor(sf::Color::Red);
    sf::Vector2f yLabelPos(gridOrigin.x + gridPan.x + 10, 20);
    if (isInGridPanel(yLabelPos)) {
        yLabel.setPosition(yLabelPos);
        window.draw(yLabel);
    }
   
    // Draw origin label
    sf::Text originLabel("O", font, 16);
    originLabel.setFillColor(sf::Color::Yellow);
    sf::Vector2f originPos(gridOrigin.x + gridPan.x + 5, gridOrigin.y + gridPan.y + 5);
    if (isInGridPanel(originPos)) {
        originLabel.setPosition(originPos);
        window.draw(originLabel);
    }
   
    // Restore the original view
    window.setView(originalView);
}

void drawVectors(sf::RenderWindow& window, const std::vector<sf::Vector2f>& vectors, sf::Font& font,
                const std::vector<std::string>& labels = {}, const std::vector<sf::Color>& colors = {}) {
    // Create a view for the grid panel to clip content
    sf::View gridView;
    gridView.reset(sf::FloatRect(LEFT_PANEL_WIDTH, 0, GRID_PANEL_WIDTH, WINDOW_HEIGHT));
    gridView.setViewport(sf::FloatRect(
        LEFT_PANEL_WIDTH / static_cast<float>(WINDOW_WIDTH),
        0,
        GRID_PANEL_WIDTH / static_cast<float>(WINDOW_WIDTH),
        1.0f
    ));
   
    // Save the original view
    sf::View originalView = window.getView();
   
    // Set the grid view for drawing vectors
    window.setView(gridView);
   
    for (size_t i = 0; i < vectors.size(); i++) {
        const auto& vec = vectors[i];
        sf::Color vecColor;
       
        // Use provided color if available, otherwise use default colors
        if (i < colors.size()) {
            vecColor = colors[i];
        } else {
            // Different colors for different vectors
            switch (i % 4) {
                case 0: vecColor = sf::Color::Blue; break;
                case 1: vecColor = sf::Color::Yellow; break;
                case 2: vecColor = sf::Color::Magenta; break;
                case 3: vecColor = sf::Color::Cyan; break;
            }
        }
       
        sf::Vector2f startPoint = toSFMLCoords({0, 0});
        sf::Vector2f endPoint = toSFMLCoords(vec);
       
        // Draw the vector line
        sf::Vertex line[] = {
            sf::Vertex(startPoint, vecColor),
            sf::Vertex(endPoint, vecColor)
        };
        window.draw(line, 2, sf::Lines);

        // Draw arrowhead
        sf::Vector2f dir = endPoint - startPoint;
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (length > 0) {
            dir /= length;
            sf::Vector2f normal(-dir.y, dir.x);
            sf::Vector2f arrowPoint = endPoint;
            sf::Vector2f arrowBase1 = arrowPoint - dir * 10.f + normal * 5.f;
            sf::Vector2f arrowBase2 = arrowPoint - dir * 10.f - normal * 5.f;
           
            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setPoint(0, arrowPoint);
            arrow.setPoint(1, arrowBase1);
            arrow.setPoint(2, arrowBase2);
            arrow.setFillColor(vecColor);
            window.draw(arrow);
        }

        // Use provided label if available, otherwise use default label
        std::string label;
        if (i < labels.size()) {
            label = labels[i];
        } else {
            std::ostringstream ss;
            ss << "v" << i + 1 << " (" << roundToString(vec.x) << "," << roundToString(vec.y) << ")";
            label = ss.str();
        }
       
        sf::Text labelText(label, font, 14);
        labelText.setFillColor(vecColor);
        sf::Vector2f labelPos = endPoint + sf::Vector2f(5, -20);
       
        // Ensure label stays within grid panel
        if (labelPos.x + labelText.getLocalBounds().width > LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH) {
            labelPos.x = LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH - labelText.getLocalBounds().width - 5;
        }
       
        labelText.setPosition(labelPos);
        window.draw(labelText);
    }
   
    // Restore the original view
    window.setView(originalView);
}

std::string getInputFormatHelp(int inputState) {
    switch (inputState) {
        case 1: // WAITING_VECTOR
            return "Format: x y\nExample: 2 3";
        case 2: // WAITING_VECTOR_FOR_ROTATION
            return "Format: x y\nExample: 2 3";
        case 3: // WAITING_ROTATION_ANGLE
            return "Format: angle\nExample: 45";
        case 4: // WAITING_VECTOR_FOR_SHEAR
            return "Format: x y\nExample: 2 3";
        case 5: // WAITING_SHEAR
            return "Format: shearX shearY\nExample: 0.5 0.3";
        case 6: // WAITING_VECTOR_FOR_SCALE
            return "Format: x y\nExample: 2 3";
        case 7: // WAITING_SCALE
            return "Format: scaleX scaleY\nExample: 2 1.5";
        case 8: // WAITING_VECTOR1_FOR_ADD
        case 11: // WAITING_VECTOR1_FOR_SUB
        case 14: // WAITING_VECTOR1_FOR_PROJ
            return "Format: x y (first vector)\nExample: 2 3";
        case 9: // WAITING_VECTOR2_FOR_ADD
        case 12: // WAITING_VECTOR2_FOR_SUB
        case 15: // WAITING_VECTOR2_FOR_PROJ
            return "Format: x y (second vector)\nExample: 4 5";
        case 17: // WAITING_VECTOR_FOR_REFLECT
            return "Format: x y\nExample: 2 3";
        case 18: // WAITING_REFLECT_CHOICE
            return "Format: choice (1-4)\n1:X-axis, 2:Y-axis\n3:Origin, 4:-XY";
        default:
            return "";
    }
}

void drawInstructions(sf::RenderWindow& window, sf::Font& font, int inputState) {
    // Draw title with better styling
    sf::RectangleShape titleBg(sf::Vector2f(LEFT_PANEL_WIDTH, 60));
    titleBg.setFillColor(sf::Color(50, 50, 80));
    window.draw(titleBg);
   
    sf::Text title("Matrix Visualizer", font, 24);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color::White);
    title.setPosition(LEFT_PANEL_WIDTH / 2 - title.getLocalBounds().width / 2, 15);
    window.draw(title);
   
    // Draw controls
    std::vector<std::string> lines = {
        "=== Controls ===",
        "",
        "V : Add Vector",
        "R : Rotate Vector",
        "H : Shear Vector",
        "S : Scale Vector",
        "A : Add two vectors",
        "N : Subtract two vectors",
        "P : Project vector", // Changed from M to P
        "F : Reflect Vector",
        "SPACE : Reset vectors",
        "",
        "Arrow Keys : Pan grid",
        "Z : Zoom in",
        "X : Zoom out",
        "C : Reset view",
        "",
        "ESC : Quit"
    };

    for (size_t i = 0; i < lines.size(); ++i) {
        sf::Text text(lines[i], font, 16);
        text.setFillColor(sf::Color::White);
        text.setPosition(20, 70 + i * 24);
        window.draw(text);
    }
   
    // Draw input format help if applicable
    std::string formatHelp = getInputFormatHelp(inputState);
    if (!formatHelp.empty()) {
        sf::RectangleShape helpBg(sf::Vector2f(LEFT_PANEL_WIDTH, 120));
        helpBg.setPosition(0, WINDOW_HEIGHT - 120);
        helpBg.setFillColor(sf::Color(60, 60, 60));
        window.draw(helpBg);
       
        sf::Text helpTitle("Input Format:", font, 16);
        helpTitle.setStyle(sf::Text::Bold);
        helpTitle.setFillColor(sf::Color::Yellow);
        helpTitle.setPosition(20, WINDOW_HEIGHT - 115);
        window.draw(helpTitle);
       
        sf::Text helpText(formatHelp, font, 14);
        helpText.setFillColor(sf::Color::White);
        helpText.setPosition(20, WINDOW_HEIGHT - 90);
        window.draw(helpText);
    }
}

void drawInputPanel(sf::RenderWindow& window, sf::Font& font,
                   const std::string& currentMessage,
                   const std::string& inputBuffer,
                   const std::vector<std::string>& messageHistory) {
   
    // Draw title
    sf::RectangleShape titleBg(sf::Vector2f(RIGHT_PANEL_WIDTH, 60));
    titleBg.setPosition(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH, 0);
    titleBg.setFillColor(sf::Color(50, 50, 80));
    window.draw(titleBg);
   
    sf::Text title("Input Panel", font, 24);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color::White);
    float titleX = LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH + RIGHT_PANEL_WIDTH / 2 - title.getLocalBounds().width / 2;
    title.setPosition(titleX, 15);
    window.draw(title);
   
    // Draw current message
    sf::Text message(currentMessage, font, 16);
    message.setFillColor(sf::Color::Yellow);
    message.setPosition(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH + 20, 70);
    window.draw(message);
   
    // Draw input box
    sf::RectangleShape inputBox(sf::Vector2f(RIGHT_PANEL_WIDTH - 40, 30));
    inputBox.setPosition(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH + 20, 100);
    inputBox.setFillColor(sf::Color(50, 50, 50));
    inputBox.setOutlineColor(sf::Color::White);
    inputBox.setOutlineThickness(1);
    window.draw(inputBox);
   
    // Draw input text
    sf::Text inputText(inputBuffer + "_", font, 16);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH + 25, 105);
    window.draw(inputText);
   
    // Draw message history
    sf::Text historyTitle("Message History:", font, 16);
    historyTitle.setStyle(sf::Text::Bold);
    historyTitle.setFillColor(sf::Color::White);
    historyTitle.setPosition(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH + 20, 150);
    window.draw(historyTitle);
   
    for (size_t i = 0; i < messageHistory.size() && i < 10; ++i) {
        sf::Text historyText(messageHistory[messageHistory.size() - 1 - i], font, 14);
        historyText.setFillColor(sf::Color(200, 200, 200));
        historyText.setPosition(LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH + 20, 180 + i * 20);
        window.draw(historyText);
    }
}

sf::Vector2f parseVector(const std::string& input) {
    std::istringstream ss(input);
    float x, y;
    if (ss >> x >> y) {
        return {x, y};
    }
    throw std::runtime_error("Invalid vector format");
}

Matrix<float> vectorToMatrix(const sf::Vector2f& vec) {
    Matrix<float> mat(2, 1);
    mat.setElement(0, 0, vec.x);
    mat.setElement(1, 0, vec.y);
    return mat;
}

sf::Vector2f matrixToVector(const Matrix<float>& mat) {
    if (mat.getRows() >= 2 && mat.getCols() >= 1) {
        return {mat.getElement(0, 0), mat.getElement(1, 0)};
    }
    throw std::runtime_error("Matrix cannot be converted to vector");
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Matrix Visualizer");

    sf::Font font;
    if (!font.loadFromFile("../font/Roboto-Regular.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    std::vector<sf::Vector2f> vectors = {{2, 1}, {1, 2}};
    std::vector<std::string> vectorLabels;
    std::vector<sf::Color> vectorColors;
   
    std::string inputBuffer;
    std::string currentMessage = "Press a key to select an operation";
    std::vector<std::string> messageHistory;
   
    // Temporary storage for operation vectors
    sf::Vector2f tempVector1;
    sf::Vector2f tempVector2;
    sf::Vector2f resultVector;
   
    enum InputState {
        WAITING_COMMAND = 0,
        WAITING_VECTOR = 1,
        WAITING_VECTOR_FOR_ROTATION = 2,
        WAITING_ROTATION_ANGLE = 3,
        WAITING_VECTOR_FOR_SHEAR = 4,
        WAITING_SHEAR = 5,
        WAITING_VECTOR_FOR_SCALE = 6,
        WAITING_SCALE = 7,
        WAITING_VECTOR1_FOR_ADD = 8,
        WAITING_VECTOR2_FOR_ADD = 9,
        WAITING_ADD_RESULT = 10,
        WAITING_VECTOR1_FOR_SUB = 11,
        WAITING_VECTOR2_FOR_SUB = 12,
        WAITING_SUB_RESULT = 13,
        WAITING_VECTOR1_FOR_PROJ = 14, // Changed from MULT to PROJ
        WAITING_VECTOR2_FOR_PROJ = 15, // Changed from MULT to PROJ
        WAITING_PROJ_RESULT = 16,      // Changed from MULT to PROJ
        WAITING_VECTOR_FOR_REFLECT = 17,
        WAITING_REFLECT_CHOICE = 18
    };
   
    InputState inputState = WAITING_COMMAND;
   
    // For smooth panning
    bool isPanning = false;
    sf::Vector2f lastMousePos;
   
    // For temporary vector display
    std::vector<sf::Vector2f> displayVectors;
    std::vector<std::string> displayLabels;
    std::vector<sf::Color> displayColors;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // Handle text input
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !inputBuffer.empty()) {
                    inputBuffer.pop_back();
                } else if (event.text.unicode == '\r' || event.text.unicode == '\n') { // Enter key
                    messageHistory.push_back("> " + inputBuffer);
                   
                    try {
                        if (inputState == WAITING_VECTOR) {
                            sf::Vector2f newVector = parseVector(inputBuffer);
                            vectors.push_back(newVector);
                            messageHistory.push_back("Vector added: (" + roundToString(newVector.x) + "," + roundToString(newVector.y) + ")");
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                        }
                        else if (inputState == WAITING_VECTOR_FOR_ROTATION) {
                            tempVector1 = parseVector(inputBuffer);
                            messageHistory.push_back("Vector to rotate: (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            inputState = WAITING_ROTATION_ANGLE;
                            currentMessage = "Enter rotation angle in degrees:";
                           
                            // Display the vector to rotate
                            displayVectors = {tempVector1};
                            displayLabels = {"Vector to rotate (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")"};
                            displayColors = {sf::Color::Blue};
                        }
                        else if (inputState == WAITING_ROTATION_ANGLE) {
                            float angle;
                            std::istringstream ss(inputBuffer);
                            if (ss >> angle) {
                                messageHistory.push_back("Rotating vector by " + std::to_string(angle) + " degrees");
                               
                                // Use the RotateMatrix class from matrix.hpp
                                RotateMatrix<float> rotMat(angle);
                                Matrix<float> vecMat = vectorToMatrix(tempVector1);
                                Matrix<float> result = rotMat * vecMat;
                                resultVector = matrixToVector(result);
                               
                                // Clear previous vectors and add only original and result
                                vectors.clear();
                                vectors.push_back(tempVector1);
                                vectors.push_back(resultVector);
                               
                                // Add labels
                                vectorLabels.clear();
                                vectorLabels.push_back("Original (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                                vectorLabels.push_back("Rotated (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                               
                                // Add colors
                                vectorColors.clear();
                                vectorColors.push_back(sf::Color::Blue);
                                vectorColors.push_back(sf::Color::Green);
                               
                                // Clear display vectors
                                displayVectors.clear();
                                displayLabels.clear();
                                displayColors.clear();
                            } else {
                                messageHistory.push_back("Error: Invalid angle format");
                            }
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                        }
                        else if (inputState == WAITING_VECTOR_FOR_SHEAR) {
                            tempVector1 = parseVector(inputBuffer);
                            messageHistory.push_back("Vector to shear: (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            inputState = WAITING_SHEAR;
                            currentMessage = "Enter shear factors as 'shearX shearY':";
                           
                            // Display the vector to shear
                            displayVectors = {tempVector1};
                            displayLabels = {"Vector to shear (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")"};
                            displayColors = {sf::Color::Blue};
                        }
                        else if (inputState == WAITING_SHEAR) {
                            float shx, shy;
                            std::istringstream ss(inputBuffer);
                            if (ss >> shx >> shy) {
                                messageHistory.push_back("Shearing vector by X:" + roundToString(shx) + " Y:" + roundToString(shy));
                               
                                // Use the ShearMatrix class from matrix.hpp
                                ShearMatrix<float> shearMat(2, 2, shx, shy);
                                Matrix<float> vecMat = vectorToMatrix(tempVector1);
                                Matrix<float> result = shearMat * vecMat;
                                resultVector = matrixToVector(result);
                               
                                // Clear previous vectors and add only original and result
                                vectors.clear();
                                vectors.push_back(tempVector1);
                                vectors.push_back(resultVector);
                               
                                // Add labels
                                vectorLabels.clear();
                                vectorLabels.push_back("Original (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                                vectorLabels.push_back("Sheared (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                               
                                // Add colors
                                vectorColors.clear();
                                vectorColors.push_back(sf::Color::Blue);
                                vectorColors.push_back(sf::Color::Green);
                               
                                // Clear display vectors
                                displayVectors.clear();
                                displayLabels.clear();
                                displayColors.clear();
                            } else {
                                messageHistory.push_back("Error: Invalid shear format");
                            }
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                        }
                        else if (inputState == WAITING_VECTOR_FOR_SCALE) {
                            tempVector1 = parseVector(inputBuffer);
                            messageHistory.push_back("Vector to scale: (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            inputState = WAITING_SCALE;
                            currentMessage = "Enter scale factors as 'scaleX scaleY':";
                           
                            // Display the vector to scale
                            displayVectors = {tempVector1};
                            displayLabels = {"Vector to scale (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")"};
                            displayColors = {sf::Color::Blue};
                        }
                        else if (inputState == WAITING_SCALE) {
                            float sx, sy;
                            std::istringstream ss(inputBuffer);
                            if (ss >> sx >> sy) {
                                messageHistory.push_back("Scaling vector by X:" + roundToString(sx) + " Y:" + roundToString(sy));
                               
                                // Use the ScaleMatrix class from matrix.hpp
                                ScaleMatrix<float> scaleMat(2, 2, sx, sy);
                                Matrix<float> vecMat = vectorToMatrix(tempVector1);
                                Matrix<float> result = scaleMat * vecMat;
                                resultVector = matrixToVector(result);
                               
                                // Clear previous vectors and add only original and result
                                vectors.clear();
                                vectors.push_back(tempVector1);
                                vectors.push_back(resultVector);
                               
                                // Add labels
                                vectorLabels.clear();
                                vectorLabels.push_back("Original (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                                vectorLabels.push_back("Scaled (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                               
                                // Add colors
                                vectorColors.clear();
                                vectorColors.push_back(sf::Color::Blue);
                                vectorColors.push_back(sf::Color::Green);
                               
                                // Clear display vectors
                                displayVectors.clear();
                                displayLabels.clear();
                                displayColors.clear();
                            } else {
                                messageHistory.push_back("Error: Invalid scale format");
                            }
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                        }
                        else if (inputState == WAITING_VECTOR1_FOR_ADD) {
                            tempVector1 = parseVector(inputBuffer);
                            messageHistory.push_back("First vector: (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            inputState = WAITING_VECTOR2_FOR_ADD;
                            currentMessage = "Enter second vector (x y):";
                           
                            // Display the first vector
                            displayVectors = {tempVector1};
                            displayLabels = {"Vector 1 (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")"};
                            displayColors = {sf::Color::Blue};
                        }
                        else if (inputState == WAITING_VECTOR2_FOR_ADD) {
                            tempVector2 = parseVector(inputBuffer);
                            messageHistory.push_back("Second vector: (" + roundToString(tempVector2.x) + "," + roundToString(tempVector2.y) + ")");
                           
                            // Use Matrix addition from matrix.hpp
                            Matrix<float> mat1 = vectorToMatrix(tempVector1);
                            Matrix<float> mat2 = vectorToMatrix(tempVector2);
                            Matrix<float> result = mat1 + mat2;
                            resultVector = matrixToVector(result);
                           
                            messageHistory.push_back("Sum: (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                           
                            // Clear previous vectors and add only the original and result
                            vectors.clear();
                            vectors.push_back(tempVector1);
                            vectors.push_back(tempVector2);
                            vectors.push_back(resultVector);
                           
                            // Add labels
                            vectorLabels.clear();
                            vectorLabels.push_back("Vector 1 (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            vectorLabels.push_back("Vector 2 (" + roundToString(tempVector2.x) + "," + roundToString(tempVector2.y) + ")");
                            vectorLabels.push_back("Sum (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                           
                            // Add colors
                            vectorColors.clear();
                            vectorColors.push_back(sf::Color::Blue);
                            vectorColors.push_back(sf::Color::Yellow);
                            vectorColors.push_back(sf::Color::Green);
                           
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                           
                            // Clear display vectors
                            displayVectors.clear();
                            displayLabels.clear();
                            displayColors.clear();
                        }
                        else if (inputState == WAITING_VECTOR1_FOR_SUB) {
                            tempVector1 = parseVector(inputBuffer);
                            messageHistory.push_back("First vector: (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            inputState = WAITING_VECTOR2_FOR_SUB;
                            currentMessage = "Enter second vector (x y):";
                           
                            // Display the first vector
                            displayVectors = {tempVector1};
                            displayLabels = {"Vector 1 (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")"};
                            displayColors = {sf::Color::Blue};
                        }
                        else if (inputState == WAITING_VECTOR2_FOR_SUB) {
                            tempVector2 = parseVector(inputBuffer);
                            messageHistory.push_back("Second vector: (" + roundToString(tempVector2.x) + "," + roundToString(tempVector2.y) + ")");
                           
                            // Use Matrix subtraction from matrix.hpp
                            Matrix<float> mat1 = vectorToMatrix(tempVector1);
                            Matrix<float> mat2 = vectorToMatrix(tempVector2);
                            Matrix<float> result = mat1 - mat2;
                            resultVector = matrixToVector(result);
                           
                            messageHistory.push_back("Difference: (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                           
                            // Clear previous vectors and add only the original and result
                            vectors.clear();
                            vectors.push_back(tempVector1);
                            vectors.push_back(tempVector2);
                            vectors.push_back(resultVector);
                           
                            // Add labels
                            vectorLabels.clear();
                            vectorLabels.push_back("Vector 1 (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            vectorLabels.push_back("Vector 2 (" + roundToString(tempVector2.x) + "," + roundToString(tempVector2.y) + ")");
                            vectorLabels.push_back("Difference (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                           
                            // Add colors
                            vectorColors.clear();
                            vectorColors.push_back(sf::Color::Blue);
                            vectorColors.push_back(sf::Color::Yellow);
                            vectorColors.push_back(sf::Color::Green);
                           
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                           
                            // Clear display vectors
                            displayVectors.clear();
                            displayLabels.clear();
                            displayColors.clear();
                        }
                        else if (inputState == WAITING_VECTOR1_FOR_PROJ) {
                            tempVector1 = parseVector(inputBuffer);
                            messageHistory.push_back("Vector to project onto: (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            inputState = WAITING_VECTOR2_FOR_PROJ;
                            currentMessage = "Enter vector to project (x y):";
                           
                            // Display the first vector
                            displayVectors = {tempVector1};
                            displayLabels = {"Vector 1 (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")"};
                            displayColors = {sf::Color::Blue};
                        }
                        else if (inputState == WAITING_VECTOR2_FOR_PROJ) {
                            tempVector2 = parseVector(inputBuffer);
                            messageHistory.push_back("Vector to project: (" + roundToString(tempVector2.x) + "," + roundToString(tempVector2.y) + ")");
                           
                            // Use the projection function from matrix.hpp
                            Matrix<float> mat1 = vectorToMatrix(tempVector1);
                            Matrix<float> mat2 = vectorToMatrix(tempVector2);
                            Matrix<float> result = Matrix<float>::projection(mat1, mat2);
                            resultVector = matrixToVector(result);
                           
                            messageHistory.push_back("Projection: (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                           
                            // Clear previous vectors and add only the original vectors and projection
                            vectors.clear();
                            vectors.push_back(tempVector1);
                            vectors.push_back(tempVector2);
                            vectors.push_back(resultVector);
                           
                            // Add labels
                            vectorLabels.clear();
                            vectorLabels.push_back("Vector 1 (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            vectorLabels.push_back("Vector 2 (" + roundToString(tempVector2.x) + "," + roundToString(tempVector2.y) + ")");
                            vectorLabels.push_back("Projection (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                           
                            // Add colors
                            vectorColors.clear();
                            vectorColors.push_back(sf::Color::Blue);
                            vectorColors.push_back(sf::Color::Yellow);
                            vectorColors.push_back(sf::Color::Green);
                           
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                           
                            // Clear display vectors
                            displayVectors.clear();
                            displayLabels.clear();
                            displayColors.clear();
                        }
                        else if (inputState == WAITING_VECTOR_FOR_REFLECT) {
                            tempVector1 = parseVector(inputBuffer);
                            messageHistory.push_back("Vector to reflect: (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                            inputState = WAITING_REFLECT_CHOICE;
                            currentMessage = "Enter reflection choice (1:X-axis, 2:Y-axis, 3:Origin, 4:-XY):";
                           
                            // Display the vector to reflect
                            displayVectors = {tempVector1};
                            displayLabels = {"Vector to reflect (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")"};
                            displayColors = {sf::Color::Blue};
                        }
                        else if (inputState == WAITING_REFLECT_CHOICE) {
                            int choice;
                            std::istringstream ss(inputBuffer);
                            if (ss >> choice) {
                                bool reflectX = false, reflectY = false;
                               
                                switch (choice) {
                                    case 1: // X-axis
                                        reflectX = false;
                                        reflectY = true;
                                        messageHistory.push_back("Reflecting across X-axis");
                                        break;
                                    case 2: // Y-axis
                                        reflectX = true;
                                        reflectY = false;
                                        messageHistory.push_back("Reflecting across Y-axis");
                                        break;
                                    case 3: // XY (origin)
                                        reflectX = true;
                                        reflectY = true;
                                        messageHistory.push_back("Reflecting across origin");
                                        break;
                                    case 4: // -XY (negative)
                                        reflectX = true;
                                        reflectY = true;
                                        messageHistory.push_back("Reflecting across -XY");
                                        break;
                                    default:
                                        messageHistory.push_back("Invalid choice, no reflection applied");
                                        inputState = WAITING_COMMAND;
                                        currentMessage = "Press a key to select an operation";
                                        break;
                                }
                               
                                if (choice >= 1 && choice <= 4) {
                                    // Use the ReflectMatrix class from matrix.hpp
                                    ReflectMatrix<float> reflectMat(2, 2, reflectX, reflectY);
                                    Matrix<float> vecMat = vectorToMatrix(tempVector1);
                                    Matrix<float> result;
                                   
                                    if (choice == 4) { // Special case for -XY
                                        // For -XY reflection, we need to rotate by 90 degrees after reflection
                                        RotateMatrix<float> rotMat(90);
                                        result = rotMat * reflectMat * vecMat;
                                    } else {
                                        result = reflectMat * vecMat;
                                    }
                                   
                                    resultVector = matrixToVector(result);
                                   
                                    // Clear previous vectors and add only original and result
                                    vectors.clear();
                                    vectors.push_back(tempVector1);
                                    vectors.push_back(resultVector);
                                   
                                    // Add labels
                                    vectorLabels.clear();
                                    vectorLabels.push_back("Original (" + roundToString(tempVector1.x) + "," + roundToString(tempVector1.y) + ")");
                                    vectorLabels.push_back("Reflected (" + roundToString(resultVector.x) + "," + roundToString(resultVector.y) + ")");
                                   
                                    // Add colors
                                    vectorColors.clear();
                                    vectorColors.push_back(sf::Color::Blue);
                                    vectorColors.push_back(sf::Color::Green);
                                }
                               
                                // Clear display vectors
                                displayVectors.clear();
                                displayLabels.clear();
                                displayColors.clear();
                            } else {
                                messageHistory.push_back("Error: Invalid choice");
                            }
                            inputState = WAITING_COMMAND;
                            currentMessage = "Press a key to select an operation";
                        }
                    } catch (const std::exception& e) {
                        messageHistory.push_back("Error: " + std::string(e.what()));
                        inputState = WAITING_COMMAND;
                        currentMessage = "Press a key to select an operation";
                       
                        // Clear display vectors
                        displayVectors.clear();
                        displayLabels.clear();
                        displayColors.clear();
                    }
                   
                    inputBuffer.clear();
                } else if (event.text.unicode < 128) {
                    inputBuffer += static_cast<char>(event.text.unicode);
                }
            }

            // Handle key presses for commands
            if (event.type == sf::Event::KeyPressed) {
                if (inputState == WAITING_COMMAND) {
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        case sf::Keyboard::V:
                            inputState = WAITING_VECTOR;
                            currentMessage = "Enter vector as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::R:
                            inputState = WAITING_VECTOR_FOR_ROTATION;
                            currentMessage = "Enter vector to rotate as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::H:
                            inputState = WAITING_VECTOR_FOR_SHEAR;
                            currentMessage = "Enter vector to shear as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::S:
                            inputState = WAITING_VECTOR_FOR_SCALE;
                            currentMessage = "Enter vector to scale as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::A: // Changed from Add to A
                            inputState = WAITING_VECTOR1_FOR_ADD;
                            currentMessage = "Enter first vector as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::N: // Changed from Subtract to N
                            inputState = WAITING_VECTOR1_FOR_SUB;
                            currentMessage = "Enter first vector as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::P: // Changed from M to P for projection
                            inputState = WAITING_VECTOR1_FOR_PROJ;
                            currentMessage = "Enter vector to project onto as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::F:
                            inputState = WAITING_VECTOR_FOR_REFLECT;
                            currentMessage = "Enter vector to reflect as 'x y':";
                            inputBuffer.clear();
                            break;
                        case sf::Keyboard::Space:
                            vectors.clear();
                            vectors = {{2, 1}, {1, 2}};
                            vectorLabels.clear();
                            vectorColors.clear();
                            messageHistory.push_back("Vectors reset to default");
                            break;
                        case sf::Keyboard::Z:
                            // Zoom in
                            gridZoom *= 1.2f;
                            gridSize = static_cast<int>(GRID_SIZE_DEFAULT * gridZoom);
                            messageHistory.push_back("Zoomed in: " + roundToString(gridZoom));
                            break;
                        case sf::Keyboard::X:
                            // Zoom out
                            gridZoom *= 0.8f;
                            gridSize = static_cast<int>(GRID_SIZE_DEFAULT * gridZoom);
                            messageHistory.push_back("Zoomed out: " + roundToString(gridZoom));
                            break;
                        case sf::Keyboard::C:
                            // Reset view
                            gridZoom = 0.75f;
                            gridSize = static_cast<int>(GRID_SIZE_DEFAULT * gridZoom);
                            gridPan = sf::Vector2f(0, 0);
                            messageHistory.push_back("View reset");
                            break;
                        default:
                            break;
                    }
                }
               
                // Handle arrow keys for panning (regardless of input state)
                const float PAN_SPEED = 20.0f;
                switch (event.key.code) {
                    case sf::Keyboard::Left:
                        gridPan.x += PAN_SPEED;
                        break;
                    case sf::Keyboard::Right:
                        gridPan.x -= PAN_SPEED;
                        break;
                    case sf::Keyboard::Up:
                        gridPan.y += PAN_SPEED;
                        break;
                    case sf::Keyboard::Down:
                        gridPan.y -= PAN_SPEED;
                        break;
                    default:
                        break;
                }
            }
           
            // Mouse panning
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // Only start panning if mouse is in the grid area
                if (event.mouseButton.x > LEFT_PANEL_WIDTH &&
                    event.mouseButton.x < LEFT_PANEL_WIDTH + GRID_PANEL_WIDTH) {
                    isPanning = true;
                    lastMousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                }
            }
           
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                isPanning = false;
            }
           
            if (event.type == sf::Event::MouseMoved && isPanning) {
                sf::Vector2f currentMousePos(event.mouseMove.x, event.mouseMove.y);
                sf::Vector2f delta = currentMousePos - lastMousePos;
                gridPan += delta;
                lastMousePos = currentMousePos;
            }
        }

        window.clear(sf::Color::Black);
        drawGrid(window, font);
       
        // Draw either the temporary display vectors or the permanent vectors
        if (!displayVectors.empty()) {
            drawVectors(window, displayVectors, font, displayLabels, displayColors);
        } else {
            drawVectors(window, vectors, font, vectorLabels, vectorColors);
        }
       
        drawInstructions(window, font, static_cast<int>(inputState));
        drawInputPanel(window, font, currentMessage, inputBuffer, messageHistory);
        window.display();
    }

    return 0;
}