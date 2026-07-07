#include "window/window.h"

window_c::window_c(const char* name, const uint16_t width, const uint16_t height){
    if(!width || !height) return;
    this->name = name; this->height = height; this->width = width;
}

GLFWwindow* window_c::create(){
    glfwWindowHint(GLFW_SAMPLES, 4); //4x antialiasing
    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = NULL;
    GLFWwindow* share = NULL;

    window = glfwCreateWindow(width, height, name, monitor, share);
    if(!window) return nullptr;

    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    window_c::clearscreen();
    glfwSwapBuffers(window);

    return window;
}

void window_c::clearscreen(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   //black
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_c::handleInput(){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

uint16_t window_c::getHeight(){ return this->height; }
uint16_t window_c::getWidth(){ return this->width; }

void window_c::setHeight(uint16_t new_height){ this->height = new_height; }
void window_c::setWidth(uint16_t new_width){ this->width = new_width; }
