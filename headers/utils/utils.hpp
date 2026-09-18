#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <limits>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

#include "Exceptions.hpp"


std::string
getFileContent(
    const std::string& file_name
){
    if(file_name.empty()) logException(
        Severity::CRITICAL, Category::IO,
        "File not provided"
    );

    std::ifstream in(file_name, std::ios::binary);
    if(!in) throw (errno);

    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
}

