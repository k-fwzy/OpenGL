#ifndef GLWINDOW_H
#define GL_WINDOW_H

    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
    
    class window_c{
    private:
        GLFWwindow* window;
        const char* name;
        uint16_t width{0};
        uint16_t height{0};
    public:
        window_c(const char* name, const uint16_t width, uint16_t height);
        GLFWwindow* create();
        void clearscreen();

        uint16_t getWidth();
        uint16_t getHeight();

        void setWidth(uint16_t new_width);
        void setHeight(uint16_t new_height);

        void handleInput();
    };

#endif
