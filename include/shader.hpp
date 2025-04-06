// Shader.hpp
#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <GL/glew.h> // Use GLAD if you prefer
#include <glm/glm.hpp>

class Shader {
public:
    GLuint ID; // Program ID

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;

    // Utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    std::string readFile(const char* filePath);
    void checkCompileErrors(GLuint shader, std::string type);
    void checkLinkErrors(GLuint program);
};

#endif // SHADER_HPP