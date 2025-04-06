// Shader.cpp
#include "../include/shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector> // For error logs

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCodeStr = readFile(vertexPath);
    std::string fragmentCodeStr = readFile(fragmentPath);
    const char* vShaderCode = vertexCodeStr.c_str();
    const char* fShaderCode = fragmentCodeStr.c_str();

    // 2. Compile shaders
    GLuint vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkLinkErrors(ID);

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

std::string Shader::readFile(const char* filePath) {
     std::ifstream shaderFile;
     // Ensure ifstream objects can throw exceptions:
     shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
     try {
         shaderFile.open(filePath);
         std::stringstream shaderStream;
         shaderStream << shaderFile.rdbuf();
         shaderFile.close();
         return shaderStream.str();
     } catch (std::ifstream::failure& e) {
         std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << filePath << "\n" << e.what() << std::endl;
         return "";
     }
}


void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> infoLog(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, infoLog.data());
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog.data() << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void Shader::checkLinkErrors(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> infoLog(logLength);
        glGetProgramInfoLog(program, logLength, NULL, infoLog.data());
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog.data() << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}


// Utility uniform functions implementations... (mostly boilerplate glUniform calls)
void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    // Use mat3 for 2D transformations (apply to vec3(x, y, 1))
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}