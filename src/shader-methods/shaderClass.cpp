#include "shaders/shaderClass.h"

std::string getFileContent(const char* file_name){
    std::ifstream in(file_name, std::ios::binary);
    if(in){
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    throw(errno);
}

shader::shader(const char* vertex_file, const char* fragment_file){
    std::string vertex_code = getFileContent(vertex_file);
    std::string fragment_code = getFileContent(fragment_file);

    const char* vertex_source = vertex_code.c_str();
    const char* fragment_source = fragment_code.c_str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);

    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);

    glLinkProgram(ID);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void shader::activate(){
    glUseProgram(ID);
}

void shader::destroy(){
    glDeleteProgram(ID);
}