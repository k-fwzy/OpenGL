#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <cmath>
#include <vector>

#include "Exceptions.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "DynamicMesh.hpp"
#include "StaticMesh.hpp"
#include "Renderer.hpp"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

void 
eCb(
    int e, const char* descr
){
    throwException(
        Severity::CRITICAL, Category::RUNTIME, 
        std::string("GLFW error " + std::to_string(e) + ": ") + descr
    );
}

static const std::vector<Vertex> k_cube_verts = {
    {{ -0.5f, -0.5f, -0.5f }, { 1.f, 0.f, 0.f }},
    {{  0.5f, -0.5f, -0.5f }, { 0.f, 1.f, 0.f }},
    {{  0.5f,  0.5f, -0.5f }, { 1.f, 1.f, 0.f }},
    {{ -0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 1.f }},
    {{ -0.5f, -0.5f,  0.5f }, { 0.f, 1.f, 1.f }},
    {{  0.5f, -0.5f,  0.5f }, { 1.f, 1.f, 1.f }},
    {{  0.5f,  0.5f,  0.5f }, { 1.f, 1.f, 1.f }},
    {{ -0.5f,  0.5f,  0.5f }, { 1.f, 1.f, 1.f }},
};

static const std::vector<uint32_t> k_cube_idx = {
    0, 2, 1,  0, 3, 2,
    4, 5, 6,  4, 6, 7,
    0, 7, 3,  0, 4, 7,
    1, 2, 6,  1, 6, 5,
    0, 1, 5,  0, 5, 4,
    3, 7, 6,  3, 6, 2,
};

int
main(void){
    glfwSetErrorCallback(eCb);
    try{
        Window win("hi", WIN_WIDTH, WIN_HEIGHT);
        win.makeContextCurrent();
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            throwException(
                Severity::CRITICAL, Category::RENDERER, 
                "Glad failed to load"
            );
        }
        Shader shader("default.vert", "default.frag");
        Renderer renderer;

        StaticMesh cube(k_cube_verts, k_cube_idx, defaultLayout());

        while(!win.shouldClose()){
            glfwPollEvents();

            int fb_w = 0, fb_h = 0;
            glfwGetFramebufferSize(win.handle(), &fb_w, &fb_h);
            renderer.beginFrame(fb_w, fb_h);
                const float aspect = fb_h > 0 ? 
                    static_cast<float>(fb_w) / static_cast<float>(fb_h) 
                : 1.0f;

                glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
                glm::mat4 view = glm::lookAt(
                    glm::vec3(0.0f, 0.0f, 3.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );
                const float t = static_cast<float>(glfwGetTime());
                glm::mat4 model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.5f, 1.0f, 0.0f));
                glm::mat4 mvp = proj * view * model;

                shader.use();
                shader.setMat4("uMVP", mvp);

                cube.draw();
            renderer.endFrame();
            win.swapBuffers();
        }

    }catch(const Exception& e){
        const auto& info = e.error();
        std::cerr << "[" << toString(info.severity) << "] "  << toString(info.category) << ": "
        << info.message << "\n";
        for(const auto& f : info.traceback) 
            std::cerr << "|\tat " << f << "\n";
        return -1;
    }catch(const std::exception& e){
        std::cerr << "error: " << e.what() << '\n';
        return -1;
    }
    return 0;
}
