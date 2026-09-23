#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <vector>

struct
VertexAttribute {
    GLuint location;
    GLint components;
    GLenum type;
    GLboolean normalized;
    GLsizei offset; 
};

struct
VertexLayout {
    std::vector<VertexAttribute> attributes;
    GLsizei stride;
};

inline VertexLayout
defaultLayout(void){
    return VertexLayout{
        .attributes = {
            { 0, 3, GL_FLOAT, GL_FALSE, 0 },
            { 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) }
        },
        .stride = 6 * sizeof(float),
    };
}

struct 
Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

inline void 
configureVertexAttributes(
    const VertexLayout& layout
){
    for(const auto& a: layout.attributes){
        glEnableVertexAttribArray(a.location);
        glVertexAttribPointer(
            a.location, a.components, a.type, a.normalized, 
            layout.stride, reinterpret_cast<const void*>(a.offset)
        );
    }
}