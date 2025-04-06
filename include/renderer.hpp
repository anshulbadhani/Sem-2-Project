// Renderer.hpp
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.hpp"

class Renderer {
public:
    Renderer(int gridRange);
    ~Renderer();

    void setup();
    void draw(const Shader& shader, const glm::mat3& transformMatrix, const glm::mat4& projectionMatrix);

private:
    GLuint gridVAO, gridVBO;
    GLuint axesVAO, axesVBO;
    std::vector<float> gridVertices;
    std::vector<float> axesVertices;
    int gridRange;
    size_t gridVertexCount;
    size_t axesVertexCount;

    void setupGrid();
    void setupAxes();
};

#endif // RENDERER_HPP