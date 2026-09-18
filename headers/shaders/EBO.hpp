#pragma once
#include "glad/glad.h"

class EBO{
public: 
    GLuint ID{0};

    explicit 
    EBO(
        GLuint* indices, 
        GLsizeiptr size
    ){
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    }

    void bind(void){ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
    void unbind(void){ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    void destroy(void){ glDeleteBuffers(1, &ID); }

};
