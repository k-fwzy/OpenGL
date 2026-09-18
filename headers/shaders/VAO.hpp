#pragma once
#include "glad/glad.h"
#include "VBO.hpp"

class 
VAO {
public:
    GLuint ID{0};
    
    explicit 
    VAO(void){ 
        glGenVertexArrays(1, &ID); 
    }

    void 
    linkVBO(VBO vbo, GLuint layout){
        vbo.bind();
        glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(layout);
        vbo.unbind();
    }

    void bind(void){ glBindVertexArray(ID); }
    void unbind(void){ glBindVertexArray(0); };
    void destroy(void){ glDeleteVertexArrays(1, &ID); };
};
    