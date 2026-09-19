#pragma once

#include <glm/glm.hpp>
#include "Shader.hpp"
#include "StaticMesh.hpp"

class
Renderer {
private:
    glm::mat4 view_proj_{1.f};
public:
    void
    beginFrame(
        int fb_width, 
        int fb_height
    ){
        glViewport(0, 0, fb_width, fb_height);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void 
    endFrame(void){
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void
    draw(
        const StaticMesh& mesh,
        Shader& shader,
        const glm::mat4& model
    ){
        shader.use();
        shader.setMat4("uMVP", view_proj_ * model);
        mesh.draw();
    }

    void
    setViewProjection(const glm::mat4& vp){ view_proj_ = vp; }
};
