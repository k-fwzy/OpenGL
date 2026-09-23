#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "utils.hpp"

struct 
GLFWWindowDeleter {
    void operator()(GLFWwindow* w) const {
        if(w) glfwDestroyWindow(w);
    }
};

class Window{
private:
    std::string name;
    uint16_t width,
            height;
    std::unique_ptr<GLFWwindow, GLFWWindowDeleter> window;
    static constexpr uint16_t min_size = 1,
                            max_size = INT16_MAX;

    constexpr bool
    winSizeOk(
        const uint16_t w, const uint16_t h
    ){ return 
        (w >= min_size && w <= max_size) && 
        (h >= min_size && h <= max_size);
    }

public:
    explicit Window(
        const std::string& n, 
        uint16_t w, uint16_t h
    ): name(n), width(w), height(h) {
        if(!winSizeOk(w, h)) throwException(
            Severity::CRITICAL, Category::WINDOW,
            "Invalid window size: " +
            std::to_string(w) + ", " + std::to_string(h)
        );
        if(!glfwInit()) throwException(
            Severity::CRITICAL, Category::WINDOW,
            "glfwInit failed"
        );

        glfwWindowHint(GLFW_SAMPLES, 4); //4x antialiasing
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif
        GLFWmonitor* monitor = nullptr;
        GLFWwindow* share = nullptr;
        GLFWwindow* raw = glfwCreateWindow(
            width, height, name.c_str(), monitor, share 
        ); if(!raw){
            glfwTerminate();
            throwException(
                Severity::CRITICAL, Category::WINDOW,
                "glfwCreateWindow failed"
            );
        }
        window.reset(raw);
    }

    ~Window(void){
        window.reset();
        glfwTerminate();
    }
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool
    shouldClose(void) const {
        return glfwWindowShouldClose(window.get());
    }

    void 
    swapBuffers(void) const {
        glfwSwapBuffers(window.get());
    }

    void 
    makeContextCurrent(void) const {
        glfwMakeContextCurrent(window.get());
    }

    void
    clearScreen(void){
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glm::vec<2, uint16_t>
    getSize(void){
        int w = 0, h = 0;
        glfwGetWindowSize(window.get(), &w, &h);
        return {
            static_cast<uint16_t>(w), static_cast<uint16_t>(h)
        };
    }

    uint16_t
    getHeight(void){
        int h = 0;
        glfwGetWindowSize(window.get(), nullptr, &h);
        return static_cast<uint16_t>(h);
    }

    uint16_t
    getWidth(void){
        int w = 0;
        glfwGetWindowSize(window.get(), &w, nullptr);
        return static_cast<uint16_t>(w);
    }

    void
    setWindowSize(
        const uint16_t w, const uint16_t h
    ){ 
        if(!winSizeOk(w, h))[[unlikely]]{ throwException(
            Severity::MED, Category::WINDOW,
            "Invalid window size: " +
            std::to_string(w) + ", " + std::to_string(h)
        );}
        glfwSetWindowSize(window.get(), w, h);
        width = w; height = h;
    }
    void setWidth(uint16_t w){ setWindowSize(w, this->height); }
    void setHeight(uint16_t h){ setWindowSize(this->width, h); }

    GLFWwindow* 
    handle(void) const noexcept { 
        return window.get(); 
    }
};