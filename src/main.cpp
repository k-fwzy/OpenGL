#include <iostream>
#include <stdexcept>
#include <exception>
#include <cmath>
#include <vector>

#include "Exceptions.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define WIN_HEIGHT 1080 
#define WIN_WIDTH 1920

int 
main(void){
    try{
        Window win(":o", WIN_WIDTH, WIN_HEIGHT);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            logException(
                Severity::CRITICAL, Category::RENDERER, 
                "Glad failed to load"
            );
        }
        Shader shader_program("default.vert", "default.frag");

        while(!win.shouldClose()){
            win.clearScreen();
            shader_program.activate();

            win.swapBuffers();
            glfwPollEvents();
        }

    }catch(const Exception& e){
        std::cerr << e.what() << "\n";
        glfwTerminate();
        return -1;
    }catch(const std::exception& e){
        std::cout << "error: " << e.what() << '\n';
        glfwTerminate();
        return -1;
    }

    glfwTerminate();
    return 0;
}
