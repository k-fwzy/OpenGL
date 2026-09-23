#include <glm/glm.hpp> /* https://github.com/g-truc/glm */
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>  /* https://github.com/dav1dde/glad */
#include <GLFW/glfw3.h> /* https://github.com/glfw/glfw */
#include <iostream>
#include <stdexcept>
#include <exception>
#include <cmath>
#include <vector>

#include "Exceptions.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Text.hpp"
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
    {{  0.5f, -0.5f, -0.5f }, { 1.f, 0.f, 0.f }},
    {{  0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f }},
    {{ -0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f }},
    {{ -0.5f, -0.5f,  0.5f }, { 1.f, 0.4f, 0.4f }},
    {{  0.5f, -0.5f,  0.5f }, { 1.f, 0.4f, 0.4f }},
    {{  0.5f,  0.5f,  0.5f }, { 1.f, 0.4f, 0.4f }},
    {{ -0.5f,  0.5f,  0.5f }, { 1.f, 0.4f, 0.4f }},
};

static const std::vector<uint32_t> k_cube_idx = {
    0, 2, 1,  0, 3, 2,
    4, 5, 6,  4, 6, 7,
    0, 7, 3,  0, 4, 7,
    1, 2, 6,  1, 6, 5,
    0, 1, 5,  0, 5, 4,
    3, 7, 6,  3, 6, 2,
};

static std::vector<Vertex> cylinder_verts;
static std::vector<uint32_t> cylinder_idx;
void
generateCylinder(void){
    int segments = 30;
    float height = 1.f;
    float radius = 0.5f;
    float half_height = height / 2.f;

    for(int i = 0; i <= segments; ++i){
        float a = (float)i / segments * glm::two_pi<float>();
        float x = std::cos(a) * radius;
        float z = std::sin(a) * radius;

        cylinder_verts.push_back({
            { x, half_height, z },
            { 0.f, 0.f, 1.0f }
        });

        cylinder_verts.push_back({
            { x, -half_height, z }, 
            { 0.2f, 0.6f, 1.f }
        });
    }

    for(int i = 0; i < segments; ++i){
        uint32_t t0 = i * 2;
        uint32_t b0 = i * 2 + 1;
        uint32_t t1 = (i + 1) * 2;
        uint32_t b1 = (i + 1) * 2 + 1;

        cylinder_idx.insert(
            cylinder_idx.end(), 
            { t0, b0, t1,  t1, b0, b1 }
        );
    }

    uint32_t top_center = (uint32_t)cylinder_verts.size();
    cylinder_verts.push_back({ 
        { 0.f, half_height, 0.f }, 
        { 0.f, 0.f, 1.f } 
    });
    for(int i = 0; i < segments; ++i){
        uint32_t t0 = i * 2;
        uint32_t t1 = (i + 1) * 2;

        cylinder_idx.insert(
            cylinder_idx.end(), 
            { top_center, t1, t0 }
        );
    }

    uint32_t bottom_center = (uint32_t)cylinder_verts.size();
    cylinder_verts.push_back({
        { 0.f, -half_height, 0.f }, 
        { 0.2f, 0.6f, 1.f }
    });
    for(int i = 0; i < segments; ++i){
        uint32_t b0 = i * 2 + 1;
        uint32_t b1 = (i + 1) * 2 + 1;

        cylinder_idx.insert(
            cylinder_idx.end(), 
            { bottom_center, b0, b1 }
        );
    }
}

static const std::vector<Vertex> k_pyramid_verts = {
    {{ -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }},
    {{  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }},
    {{  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
    {{ -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
    {{  0.0f,  0.5f,  0.0f }, { 1.0f, 0.6f, 0.0f }},
};

static const std::vector<uint32_t> k_pyramid_idx = {
    0, 1, 4,  1, 2, 4,  2, 3, 4,  3, 0, 4,
    0, 3, 2,  0, 2, 1
};

int
main(void){
    generateCylinder();
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

        if(!gltInit())throwException(
            Severity::CRITICAL, Category::GLTEXT,
            "gltInit failed"
        );

        Shader shader("default.vert", "default.frag");
        Renderer renderer;

        StaticMesh cube(k_cube_verts, k_cube_idx, defaultLayout());
        StaticMesh cylinder(cylinder_verts, cylinder_idx, defaultLayout());
        StaticMesh pyramid(k_pyramid_verts, k_pyramid_idx, defaultLayout());
        Text label(
            "shapes!!", 
            { 1.f, 1.f, 1.f, 1.f }, {
                static_cast<float>(win.getWidth()) / 2.f,
                static_cast<float>(win.getHeight()) - 200.f,
                5.f
        });

        while(!win.shouldClose()){
            glfwPollEvents();

            int fb_w = 0, fb_h = 0;
            glfwGetFramebufferSize(win.handle(), &fb_w, &fb_h);
            renderer.beginFrame(fb_w, fb_h);
                const float aspect = fb_h > 0 ? 
                    static_cast<float>(fb_w) / static_cast<float>(fb_h) 
                : 1.0f;

                glm::mat4 proj = glm::perspective(
                    glm::radians(80.0f), 
                    aspect,
                    0.1f, 
                    100.0f
                );

                glm::mat4 view = glm::lookAt(
                    glm::vec3(0.0f, 0.0f, 3.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );
                const float t = static_cast<float>(glfwGetTime());
                glm::mat4 cube_model = glm::rotate(
                    glm::translate(
                        glm::mat4(1.f), glm::vec3(-2.f, 0.f, 0.f)
                    ),
                    t,
                    glm::vec3(0.5f, 1.f, 0.f)
                );

                glm::mat4 cyl_model = glm::rotate(
                    glm::translate(
                        glm::mat4(1.f), glm::vec3(2.f, 0.f, 0.f)
                    ),
                    t,
                    glm::vec3(0.5f, 1.f, 0.f)
                );

                glm::mat4 pyr_model = glm::rotate(
                    glm::translate(
                        glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)
                    ),
                    t,
                    glm::vec3(0.5f, 1.f, 0.f)
                );
                shader.use();

                shader.setMat4("uMVP", proj * view * cube_model);
                cube.draw();

                shader.setMat4("uMVP", proj * view * cyl_model);
                cylinder.draw();

                shader.setMat4("uMVP", proj * view * pyr_model);
                pyramid.draw();

                label.setPosition(
                    static_cast<float>(win.getWidth()) / 2.f,
                    static_cast<float>(win.getHeight()) - 200.f,
                    5.f
                );
                label.display2D();
            renderer.endFrame();
            win.swapBuffers();
        }
        gltTerminate();
    }catch(const Exception& e){
        const auto& info = e.error();
        std::cerr << "[" << toString(info.severity) << "] " << toString(info.category) << ": "
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
