#pragma once

#include <glad/glad.h>
#include <span>
#include <cstddef>
#include <utility>
#include <stdexcept>

#include "Vertex.hpp"

class 
DynamicMesh {
private:
    GLuint vao_{0},
            vbo_{0};
    std::size_t capacity_{0},
                count_{0};
    GLenum primitive_{GL_TRIANGLES};

    void
    destroy(void) noexcept {
        if(vbo_) glDeleteBuffers(1, &vbo_);
        if(vao_) glDeleteVertexArrays(1, &vao_);
        vao_ = vbo_ = 0;
    }

public:
    explicit
    DynamicMesh(
        std::size_t capacity,
        VertexLayout layout,
        GLenum primitive = GL_TRIANGLES
    ): capacity_(capacity), primitive_(primitive) {
        if(capacity == 0) throw std::invalid_argument(
            "DynamicMesh: capacity must be > 0"
        );
        
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(capacity_ * sizeof(Vertex)),
            nullptr,
            GL_DYNAMIC_DRAW
        );
        configureVertexAttributes(layout);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ~DynamicMesh(void){ destroy(); }
    DynamicMesh(const DynamicMesh&) = delete;
    DynamicMesh& operator=(DynamicMesh&) = delete;
    DynamicMesh(DynamicMesh&& o) noexcept :
        vao_(std::exchange(o.vao_, 0)),
        vbo_(std::exchange(o.vbo_, 0)),
        capacity_(std::exchange(o.capacity_, 0)),
        count_(std::exchange(o.count_, 0)),
        primitive_(std::exchange(o.primitive_, GL_TRIANGLES))
    {}

    DynamicMesh& 
    operator=(DynamicMesh&& o) noexcept {
        if(this != &o){
            destroy();
            vao_ = std::exchange(o.vao_, 0);
            vbo_ = std::exchange(o.vbo_, 0);
            capacity_ = std::exchange(o.capacity_, 0);
            count_ = std::exchange(o.count_, 0);
            primitive_ = std::exchange(o.primitive_, GL_TRIANGLES);
        }
        return *this;
    }

    void 
    update(
        std::span<const Vertex> vertices
    ){
        if(vertices.size() > capacity_) throw std::length_error(
            "DynamicMesh: update exceeds capacity"
        );
        count_ = vertices.size();

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(count_ * sizeof(Vertex)),
            nullptr,
            GL_DYNAMIC_DRAW
        );
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            static_cast<GLsizeiptr>(count_ * sizeof(Vertex)),
            vertices.data()
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void 
    draw(void) const {
        if(count_ == 0) return;
        glBindVertexArray(vao_);
        glDrawArrays(primitive_, 0, static_cast<GLsizei>(count_));
        glBindVertexArray(0);
    }

    std::size_t capacity(void) const noexcept { return capacity_; }
    std::size_t count(void) const noexcept { return count_; }
};