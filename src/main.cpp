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

void 
eCb(
    int e, const char* descr
){
    throwException(
        Severity::CRITICAL, Category::RUNTIME, 
        std::string("GLFW error " + std::to_string(e) + ": ") + descr
    );
}

/*
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
static void
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
*/

static std::vector<Vertex> ring_verts;
static std::vector<uint32_t> ring_idx;
static void
generateRing(
    float radius = 1.f
){
    int segments = 32;
    const float inner_radius = radius;
    const float outer_radius = radius + 0.005f;
    for(int i = 0; i <= segments; ++i){
        float a = glm::two_pi<float>() * (float)i / segments;
        float c = std::cos(a);
        float s = std::sin(a);

        ring_verts.push_back({ 
            { c * inner_radius, s * inner_radius, 0.f }, 
            { 1.f, 1.f, 1.f } 
        });
        ring_verts.push_back({ 
            { c * outer_radius, s * outer_radius, 0.f }, 
            { 1.f, 1.f, 1.f }
        });
    }

    for(int i = 0; i < segments; ++i){
        uint32_t in0 = i * 2;
        uint32_t out0 = i * 2 + 1;
        uint32_t in1 = (i + 1) * 2;
        uint32_t out1 = (i + 1) * 2 + 1;

        ring_idx.insert(
            ring_idx.end(),
            { in0, out0, in1,  in1, out0, out1 }
        );
    }
}

static std::vector<Vertex> sphere_verts;
static std::vector<uint32_t> sphere_idx;
static void
generateSphere(
    float radius = 1.f
){
    sphere_verts.clear();
    sphere_idx.clear();
    int stacks = 16, slices = 32;

    for(int i = 0; i <= stacks; ++i){
        float phi = glm::pi<float>() * (float)i / stacks;
        float y   = std::cos(phi) * radius;
        float r   = std::sin(phi) * radius;

        for(int j = 0; j <= slices; ++j){
            float theta = glm::two_pi<float>() * (float)j / slices;
            float x = std::cos(theta) * r;
            float z = std::sin(theta) * r;

            sphere_verts.push_back({
                { x, y, z }, {
                    0.5f + 0.5f * (x / radius),
                    0.5f + 0.5f * (y / radius),
                    0.5f + 0.5f * (z / radius),
                }
            });
        }
    }

    for(int i = 0; i < stacks; ++i){
        for(int j = 0; j < slices; ++j){
            uint32_t a = i * (slices + 1) + j;
            uint32_t b = (i + 1) * (slices + 1) + j;

            if(i != 0) sphere_idx.insert(
                sphere_idx.end(), { a, b, a + 1 }
            );
            if(i != stacks - 1) sphere_idx.insert(
                sphere_idx.end(), { b, b + 1, a + 1 }
            );
        }
    }
}

/*
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
*/

int
main(void){
    // generateCylinder();
    generateSphere();
    generateRing();
    glfwSetErrorCallback(eCb);
    try{
        Window win("hi", 1280, 720);
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

        // StaticMesh cube(k_cube_verts, k_cube_idx, defaultLayout());
        // StaticMesh cylinder(cylinder_verts, cylinder_idx, defaultLayout());
        // StaticMesh pyramid(k_pyramid_verts, k_pyramid_idx, defaultLayout());
        StaticMesh sphere(sphere_verts, sphere_idx, defaultLayout());
        StaticMesh ring(ring_verts, ring_idx, defaultLayout());

        /*
        Text label(
            "shapes!!", 
            { 1.f, 1.f, 1.f, 1.f }, {
                static_cast<float>(win.getWidth()) / 2.f,
                static_cast<float>(win.getHeight()) - 200.f,
                5.f
        });
        */

        while(!win.shouldClose()){
            glfwPollEvents();

            int fb_w = 0, fb_h = 0;
            glfwGetFramebufferSize(win.handle(), &fb_w, &fb_h);
            renderer.beginFrame(fb_w, fb_h);
                const float aspect = fb_h > 0 ? 
                    static_cast<float>(fb_w) / static_cast<float>(fb_h) 
                : 1.0f;

                glm::mat4 proj = glm::perspective(
                    glm::radians(70.0f),
                    aspect,
                    0.1f,
                    100.0f
                );

                glm::mat4 view = glm::lookAt(
                    glm::vec3(8.f, 5.0f, 5.0f),
                    glm::vec3(0.0f, -2.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );
                const float t = static_cast<float>(glfwGetTime());

/*
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
*/

                glm::mat4 planet1_model = glm::scale(
                    glm::translate(
                        glm::rotate(
                            glm::mat4(1.f),
                            t + 2,
                            glm::vec3(0.f, 1.f, 0.f)
                        ),
                        glm::vec3(3.f, -1.f, 0.f)
                    ), glm::vec3(0.3f)
                );

                glm::mat4 ring1_model = glm::scale(
                    glm::translate(
                        glm::rotate(
                            glm::mat4(1.f),
                            glm::half_pi<float>(),
                            glm::vec3(1.f, 0.f, 0.f)
                        ),
                        glm::vec3(0.f, 0.f, 1.f)
                    ), glm::vec3(3.f)
                );

                glm::mat4 planet2_model = glm::scale(
                    glm::translate(
                        glm::rotate(
                            glm::mat4(1.f),
                            t * 0.5f,
                            glm::vec3(0.f, 2.f, 0.f)
                        ),
                        glm::vec3(5.f, -1.f, 0.f)
                    ), glm::vec3(0.5f)
                );

                glm::mat4 ring2_model = glm::scale(
                    glm::translate(
                        glm::rotate(
                            glm::mat4(1.f),
                            glm::half_pi<float>(),
                            glm::vec3(1.f, 0.f, 0.f)
                        ),
                        glm::vec3(0.f, 0.f, 1.f)
                    ), glm::vec3(5.f)
                );

                glm::mat4 planet3_model = glm::scale(
                    glm::translate(
                        glm::rotate(
                            glm::mat4(1.f),
                            t * 0.2f,
                            glm::vec3(0.f, 2.f, 0.f)
                        ),
                        glm::vec3(7.f, -1.f, 0.f)
                    ), glm::vec3(0.4f)
                );

                glm::mat4 ring3_model = glm::scale(
                    glm::translate(
                        glm::rotate(
                            glm::mat4(1.f),
                            glm::half_pi<float>(),
                            glm::vec3(1.f, 0.f, 0.f)
                        ),
                        glm::vec3(0.f, 0.f, 1.f)
                    ), glm::vec3(7.f)
                );

                glm::mat4 sun_model = glm::translate(
                    glm::rotate(
                        glm::mat4(1.f),
                        t,
                        glm::vec3(0.f, 1.f, 0.f)
                    ),
                    glm::vec3(0.f, -1.f, 0.f)
                );

                shader.use();
/*
                renderer.draw(proj * view * cube_model);
                renderer.draw(proj * view * cyl_model);
                renderer.draw(proj * view * pyr_model);
*/
                renderer.draw(sphere, shader, proj * view * planet1_model);
                renderer.draw(ring, shader, proj * view * ring1_model);

                renderer.draw(sphere, shader, proj * view * planet2_model);
                renderer.draw(ring, shader, proj * view * ring2_model);

                renderer.draw(sphere, shader, proj * view * planet3_model);
                renderer.draw(ring, shader, proj * view * ring3_model);

                renderer.draw(sphere, shader, proj * view * sun_model);

/*
                label.setPosition(
                    static_cast<float>(win.getWidth()) / 2.f,
                    static_cast<float>(win.getHeight()) - 200.f,
                    5.f
                );
                label.display2D();
*/
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
