#include "ComputeShader.h"
#include <iostream>
#include <fstream>
#include <sstream>


std::string get_file_contents_duplicate(const char* fileName) {

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(fileName));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

ComputeShader::ComputeShader(const char* computeFile)
{
    std::string computeSource = get_file_contents_duplicate(computeFile);

    GLuint shader = CompileShader(computeSource);

    program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glDeleteShader(shader);

}

ComputeShader::ComputeShader(ComputeShader&& other) noexcept :
    program(other.program),
    uniformLocationCache(std::move(other.uniformLocationCache))
{
    other.program = 0;
}

ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept
{
    if (this != &other) {
        if (program) {
            glDeleteProgram(program);
        }

        program = other.program;
        uniformLocationCache = std::move(other.uniformLocationCache);

        other.program = 0;
    }
    return *this;
}

ComputeShader::~ComputeShader()
{
    if (program) {
        glDeleteProgram(program);
    }
}

void ComputeShader::Dispatch(glm::uvec3 workGroups) const
{
    glDispatchCompute(workGroups.x, workGroups.y, workGroups.z);

}

void ComputeShader::DispatchIndirect(GLuint commandBuffer) const
{
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, commandBuffer);
    glDispatchComputeIndirect(0);
}

void ComputeShader::SetMemoryBarriers(GLbitfield barriers)
{
    glMemoryBarrier(barriers);
}

void ComputeShader::SetUniform(const std::string& name, int value)
{
    GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void ComputeShader::SetUniform(const std::string& name, float value)
{
    GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void ComputeShader::SetUniform(const std::string & name, const glm::vec2 & value)
{
        GLint location = GetUniformLocation(name);
        if (location != -1) {
            glUniform2f(location, value.x, value.y);
        }
}

void ComputeShader::SetUniform(const std::string & name, const glm::vec3 & value)
{
        GLint location = GetUniformLocation(name);
        if (location != -1) {
            glUniform3f(location, value.x, value.y, value.z);
        }
}

void ComputeShader::SetUniform(const std::string & name, const glm::vec4 & value)
{
        GLint location = GetUniformLocation(name);
        if (location != -1) {
            glUniform4f(location, value.x, value.y, value.z, value.w);
        }
}

void ComputeShader::SetUniform(const std::string & name, const glm::uvec3 & value)
{
        GLint location = GetUniformLocation(name);
        if (location != -1) {
            glUniform3ui(location, value.x, value.y, value.z);
        }
}

void ComputeShader::SetUniform(const std::string & name, const glm::mat3 & value)
{
        GLint location = GetUniformLocation(name);
        if (location != -1) {
            glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
        }
}

void ComputeShader::SetUniform(const std::string & name, const glm::mat4 & value)
{
        GLint location = GetUniformLocation(name);
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        }
}

void ComputeShader::Bind()
{
    glUseProgram(program);
}

void ComputeShader::Unbind()
{
    glUseProgram(0);
}

GLuint ComputeShader::CompileShader(const std::string& source)
{
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
    const char* sourceCstr = source.c_str();
    glShaderSource(cs, 1, &sourceCstr, nullptr);
    glCompileShader(cs);

    //checking compilation
    return cs;
}

GLint ComputeShader::GetUniformLocation(const std::string& name)
{
    auto it = uniformLocationCache.find(name);
    if (it != uniformLocationCache.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        std::cerr << "WARNING : uniform" << name << " not found in the compute shader ! " << std::endl;
    }

    uniformLocationCache[name] = location;
    return location;
}
