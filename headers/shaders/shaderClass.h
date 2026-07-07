#ifndef SHADER_CLASS_HPP
    #define SHADER_CLASS_HPP
    
    #include <glad/glad.h>
    #include <string>
    #include <fstream>
    #include <sstream>
    #include <iostream>
    #include <cerrno>

    std::string getFileContent(const char* file_name);

    class shader{
        public:
        GLuint ID{0};
        shader(const char* vectex_file, const char* fragment_file);

        void activate();
        void destroy();
    };

#endif