#pragma once

#include <glad/glad.h>
#include "utils.hpp"
#include "Exceptions.hpp"

class 
Shader {
public:
    GLuint ID{0};
    explicit 
    Shader(
        const std::string& vertex_file, 
        const std::string& fragment_file
    ){
        if(vertex_file.empty() || !fragment_file.empty()) logException(
            Severity::CRITICAL, Category::SHADER,
            "Vertex or fragment file names not provided"
        );

        const char* vertex_src = getFileContent(vertex_file).c_str();
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_src, nullptr);
        glCompileShader(vertex_shader);

        const char* fragment_src = getFileContent(fragment_file).c_str();
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_src, nullptr);
        glCompileShader(fragment_shader);

        ID = glCreateProgram();
        glAttachShader(ID, vertex_shader);
        glAttachShader(ID, fragment_shader);
        glLinkProgram(ID);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void activate(void){ glUseProgram(ID); }
    ~Shader(void){
        glDeleteShader(ID);
    }
};