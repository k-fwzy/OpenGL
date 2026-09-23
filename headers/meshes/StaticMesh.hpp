#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <span>
#include <utility>

#include "VAO.hpp"
#include "Vertex.hpp"

class
StaticMesh {
private:
    GLuint vao_{0},
            vbo_{0},
            ebo_{0};
    GLsizei idx_count_{0};

    void 
    destroy(void) noexcept {
        if(ebo_) glDeleteBuffers(1, &ebo_);
        if(vbo_) glDeleteBuffers(1, &vbo_);
        if(vao_) glDeleteVertexArrays(1, &vao_);
        vao_ = vbo_ = ebo_ = 0;
    }

public:
    explicit
    StaticMesh(
        std::span<const Vertex> v,
        std::span<const uint32_t> i,
        VertexLayout layout
    ){
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(
            GL_ARRAY_BUFFER, 
            static_cast<GLsizeiptr>(v.size_bytes()),
            v.data(), GL_STATIC_DRAW
        );

        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(i.size_bytes()),
            i.data(),
            GL_STATIC_DRAW
        );
        configureVertexAttributes(layout);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        idx_count_ = static_cast<GLsizei>(i.size());
    }

    ~StaticMesh(void){ destroy(); }
    StaticMesh(const StaticMesh&) = delete;
    StaticMesh& operator=(const StaticMesh&) = delete;
    StaticMesh(StaticMesh&& o) noexcept: 
        vao_(std::exchange(o.vao_, 0)), 
        vbo_(std::exchange(o.vbo_, 0)),
        ebo_(std::exchange(o.ebo_, 0)),
        idx_count_(std::exchange(o.idx_count_, 0))
    {}

    StaticMesh& 
    operator=(StaticMesh&& o) noexcept {
        if(this != &o){
            destroy();
            vao_ = std::exchange(o.vao_, 0);
            vbo_ = std::exchange(o.vbo_, 0);
            ebo_ = std::exchange(o.ebo_, 0);
            idx_count_ = std::exchange(o.idx_count_, 0);
        }
        return *this;
    }

    void
    draw(void) const {
        glBindVertexArray(vao_);
        glDrawElements(
            GL_TRIANGLES, idx_count_, 
            GL_UNSIGNED_INT, nullptr
        );
    }
};