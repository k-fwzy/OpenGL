#pragma once
#include "glad/glad.h"

class 
VBO {
public:    
    GLuint ID{0};

    explicit
    VBO(
        GLfloat* vertices, 
        GLsizeiptr size
    ){
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);    
    }

    void bind(void){ glBindBuffer(GL_ARRAY_BUFFER, ID); }
    void unbind(void){ glBindBuffer(GL_ARRAY_BUFFER, 0); }
    void destroy(void){ glDeleteBuffers(1, &ID); }
};
