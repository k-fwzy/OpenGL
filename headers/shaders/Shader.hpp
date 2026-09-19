#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <filesystem>

#include "utils.hpp"
#include "Exceptions.hpp"

namespace fs = std::filesystem;

class 
Shader {
private:
    GLuint program_{0};
    std::unordered_map<std::string, GLint> uniform_cache_;

    void 
    destroy(void) noexcept {
        if(program_) glDeleteProgram(program_);
        program_ = 0;
        uniform_cache_.clear();
    }

    GLint 
    uniformLocation(
        const std::string& name
    ){
        std::string key(name);
        auto it = uniform_cache_.find(key);
        if(it != uniform_cache_.end()) return it->second;

        GLint location = glGetUniformLocation(program_, key.c_str());
        uniform_cache_.emplace(std::move(key), location);
        return location;
    }

    GLuint 
    compileStage(
        GLenum type, 
        const std::string& src
    ){
        GLuint shader = glCreateShader(type);
        const char* p = src.data();
        GLint len = static_cast<GLuint>(src.size());
        glShaderSource(shader, 1, &p, &len);

        glCompileShader(shader);
        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if(!ok){
            char log[1024];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            glDeleteShader(shader);
            throwException(
                Severity::CRITICAL, Category::SHADER_COMPILER, 
                std::string("Shader compilation failure: \n") + log
            );
        }
        return shader;
    }

public:
    ~Shader(void){ destroy(); }
    Shader() = default;
    explicit 
    Shader(
        const std::string& vertex, 
        const std::string& fragment
    ){
        if(vertex.empty() || fragment.empty()) throwException(
            Severity::CRITICAL, Category::IO,
            "Vertex or fragment file names not provided"
        );

        auto exeDir = fs::canonical("/proc/self/exe").parent_path();
        auto vertex_file = std::string(exeDir / ("../resources/shaders/" + vertex));
        auto fragment_file = std::string(exeDir / ("../resources/shaders/" + fragment));
        std::string vertex_content = getFileContent(vertex_file);
        std::string fragment_content = getFileContent(fragment_file);
        
        const char* vertex_src = vertex_content.c_str();
        const char* fragment_src = fragment_content.c_str();
        GLuint vertex_shader = compileStage(GL_VERTEX_SHADER, vertex_src);
        GLuint fragment_shader = compileStage(GL_FRAGMENT_SHADER, fragment_src);

        program_ = glCreateProgram();
        glAttachShader(program_, vertex_shader);
        glAttachShader(program_, fragment_shader);
        glLinkProgram(program_);

        GLint ok = GL_FALSE;
        glGetProgramiv(program_, GL_LINK_STATUS, &ok);
        if(!ok){
            char log[1024];
            glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
            glDeleteProgram(program_);
            glDeleteShader(fragment_shader); glDeleteShader(vertex_shader);
            throwException(
                Severity::CRITICAL, Category::SHADER,
                std::string("Shader program linkage failure: ") + log
            );
        }
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& o) noexcept:
        program_(std::exchange(o.program_, 0)),
        uniform_cache_(std::move(o.uniform_cache_))
    {}

    Shader& 
    operator=(Shader&& o) noexcept {
        if(this != &o){
            destroy();
            program_ = std::exchange(o.program_, 0);
            uniform_cache_ = std::move(o.uniform_cache_);
        }
        return *this;
    }

    void use(void) const { glUseProgram(program_); }
    GLuint id(void) const noexcept { return program_; }

    void
    setMat4(
        const std::string& name, 
        const glm::mat4& m
    ){
        GLint location = uniformLocation(name);
        if(location >= 0) glUniformMatrix4fv(
            location, 1, GL_FALSE, glm::value_ptr(m)
        );
    }

    void
    setVec3(
        const std::string& name,
        const glm::vec3& v
    ){
        GLint location = uniformLocation(name);
        if(location >= 0) glUniform3fv(
            location, 1, glm::value_ptr(v)
        );
    }

    void
    setFloat(
        const std::string& name, float f
    ){ 
        GLint location = uniformLocation(name);
        if(location >= 0) glUniform1f(location, f);
    }
};
