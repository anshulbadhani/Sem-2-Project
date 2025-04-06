// Renderer.cpp
#include "../include/renderer.hpp"
#include <glm/gtc/type_ptr.hpp> // For glm::value_ptr
#include <iostream>

Renderer::Renderer(int range) : gridRange(range), gridVAO(0), gridVBO(0), axesVAO(0), axesVBO(0), gridVertexCount(0), axesVertexCount(0) {
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &axesVAO);
    glDeleteBuffers(1, &axesVBO);
}

void Renderer::setup() {
    setupGrid();
    setupAxes();
}

void Renderer::setupGrid() {
    gridVertices.clear();
    float step = 1.0f;
    float rangeF = static_cast<float>(gridRange);

    // Vertical lines
    for (float x = -rangeF; x <= rangeF; x += step) {
        gridVertices.push_back(x); gridVertices.push_back(-rangeF); // Start point
        gridVertices.push_back(x); gridVertices.push_back(rangeF);  // End point
    }
    // Horizontal lines
    for (float y = -rangeF; y <= rangeF; y += step) {
        gridVertices.push_back(-rangeF); gridVertices.push_back(y); // Start point
        gridVertices.push_back(rangeF);  gridVertices.push_back(y); // End point
    }
    gridVertexCount = gridVertices.size() / 2; // 2 floats per vertex (x, y)

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::setupAxes() {
    float rangeF = static_cast<float>(gridRange);
    axesVertices = {
        // X-axis (Red) - draw slightly thicker later if needed
        -rangeF, 0.0f,
         rangeF, 0.0f,
        // Y-axis (Green)
         0.0f, -rangeF,
         0.0f,  rangeF,
        // i-hat vector (Red - brighter/thicker) - Origin to (1,0)
         0.0f, 0.0f,
         1.0f, 0.0f,
        // j-hat vector (Green - brighter/thicker) - Origin to (0,1)
         0.0f, 0.0f,
         0.0f, 1.0f
    };
    axesVertexCount = axesVertices.size() / 2;

    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);

    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, axesVertices.size() * sizeof(float), axesVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Renderer::draw(const Shader& shader, const glm::mat3& transformMatrix, const glm::mat4& projectionMatrix) {
    shader.use();
    shader.setMat4("projection", projectionMatrix);
    shader.setMat3("transform", transformMatrix); // Pass the 2D transform (as mat3)

    // Draw Grid (Light Gray)
    shader.setVec3("lineColor", 0.3f, 0.3f, 0.3f); // Set color via uniform
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertexCount);
    glBindVertexArray(0);

    // Draw Axes (X:Red, Y:Green)
    glLineWidth(2.0f); // Make axes slightly thicker

    // Draw X/Y Axes (using first 4 vertices)
    shader.setVec3("lineColor", 0.5f, 0.5f, 0.5f); // Dimmer color for main axes
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, 4); // Draw first 2 lines (X and Y axes)

    // Draw i-hat (Brighter Red) - using vertices 4, 5
     shader.setVec3("lineColor", 1.0f, 0.2f, 0.2f);
     glLineWidth(4.0f); // Make basis vectors thick
     glDrawArrays(GL_LINES, 4, 2);

    // Draw j-hat (Brighter Green) - using vertices 6, 7
     shader.setVec3("lineColor", 0.2f, 1.0f, 0.2f);
     glDrawArrays(GL_LINES, 6, 2);

    glLineWidth(1.0f); // Reset line width
    glBindVertexArray(0);
}