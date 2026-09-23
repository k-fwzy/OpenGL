#pragma once

#include <memory>

#define GLT_IMPLEMENTATION
#include <glad/glad.h>
#include <gltext.h> /* https://github.com/vallentin/glText */
#include "utils.hpp"

struct TextColor { float r, g, b, a; };
struct TextPos   { float x, y, scale; };

struct 
GLTtextDeleter {
    void 
    operator()(GLTtext* t) const { 
        if(t) gltDestroyText(t); 
    }
};

class 
Text {
private:
    TextColor color;
    TextPos   position;
    std::unique_ptr<GLTtext, GLTtextDeleter> text;
public:
    explicit Text(
        const std::string& txt,
        TextColor c, TextPos p
    ): color(c), position(p), text(gltCreateText(), GLTtextDeleter{}) {
        gltSetText(text.get(), txt.c_str());
    }

    void
    display2D(void){
        gltColor(color.r, color.g, color.b, color.a);
        gltBeginDraw();
        gltDrawText2D(text.get(), position.x, position.y, position.scale);
        gltEndDraw();
    }

};