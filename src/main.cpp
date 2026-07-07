#include <iostream>
#include <stdexcept>
#include <exception>
#include <cmath>
#include <vector>

#include "shaders/EBO.h"
#include "shaders/VBO.h"
#include "shaders/VAO.h"
#include "shaders/shaderClass.h"
#include "window/window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WIN_HEIGHT 1080
#define WIN_WIDTH 1920

class mesh{
    public:
    mesh(float vertices[]){

    }
};

int main(void){
    glfwInit();
    try{
        window_c window_instance("opengl babyyy", WIN_WIDTH, WIN_HEIGHT);
        GLFWwindow* window = window_instance.create();
        if(!window){
            throw std::runtime_error("could not initialize window");
        }

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            glfwDestroyWindow(window);
            throw std::runtime_error("glad loader failed");
        }

        mesh* triangle = new mesh({
            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f
        });
        if(!triangle){
            throw std::runtime_error("could not initialize triangle"); 
        }

        shader shader_program("default.vert", "default.frag");

        VAO vao1; vao1.bind();
        VBO vbo1(triangle->vertices, sizeof(triangle->vertices)); vbo1.bind();
        EBO ebo1(triangle->indices, sizeof(triangle->indices)); ebo1.bind();
        
        vao1.linkVBO(vbo1, 0);
        vao1.unbind(); vbo1.unbind(); ebo1.unbind();

        mesh* triangle = new mesh(&v_triangle);
        if(!triangle){
            throw std::runtime_error("could not initialize mesh");
        }

        while(!glfwWindowShouldClose(window)){
            window_instance.clearscreen();

            shader_program.activate();

            glBindVertexArray(triangle->VAO);
            glDrawElements(
                GL_TRIANGLES, 
                triangle->vertex_count,
                GL_UNSIGNED_INT, 
                (void*) 0 //byte offset: 1
            );
            vao1.bind();

            window_instance.handleInput();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        vao1.destroy(); vbo1.destroy(); ebo1.destroy();
        shader_program.destroy();

        glfwDestroyWindow(window);

    }catch(const std::exception& e){
        std::cout << "error: " << e.what() << '\n';
        glfwTerminate();
        return -1;
    }

    glfwTerminate();
    return 0;
}
