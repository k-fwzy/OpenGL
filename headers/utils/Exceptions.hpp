#pragma once

#include <string>
#include <vector>
#include <exception>
#include <utility>
#include <source_location>

enum class
Severity {
    CRITICAL, HIGH, MED, LOW,
};

enum class
Category {
    WINDOW, IO, SHADER, RUNTIME, GLAD, RENDERER,
    SHADER_COMPILER, GLTEXT,
};

struct 
ErrInfo {
    Severity severity;
    Category category;
    std::string message;
    std::vector<std::string> traceback;
};

std::string
toString(Severity s){
    switch(s){
        case Severity::CRITICAL: return "CRITICAL";
        case Severity::HIGH: return "ERROR";
        case Severity::MED: return "WARNING";
        case Severity::LOW: return "INFO";
    }
    return "???";
}

std::string
toString(Category c){
    switch(c){
        case Category::GLAD: return "GLAD";
        case Category::IO: return "IOsException";
        case Category::SHADER: return "ShaderException";
        case Category::RUNTIME: return "RuntimeException";
        case Category::WINDOW: return "WindowException";
        case Category::RENDERER: return "RenderException";
        case Category::SHADER_COMPILER: return "ShaderCompilationException";
        case Category::GLTEXT: return "GLT";
    }
    return "???";
}

class
Exception: public std::exception {
private:
    ErrInfo info;
public:
    explicit Exception(ErrInfo e): info(std::move(e)) {}
    const char* what(void) const noexcept override { return info.message.c_str(); }
    const ErrInfo& error(void) const noexcept { return info; }
};

inline void
throwException(
    Severity severity,
    Category category,
    const std::string& message,
    std::source_location loc = std::source_location::current()
){
    ErrInfo info;
    info.severity = severity;
    info.category = category;
    info.message = message;
    info.traceback.push_back(
        std::string(loc.file_name()) + ":" +
        std::to_string(loc.line()) + " in " + loc.function_name()
    );
    throw Exception(std::move(info));
}
