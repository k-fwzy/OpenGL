#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include "Exceptions.hpp"

std::string
getFileContent(
    const std::string& file_name
){
    if(file_name.empty()) throwException(
        Severity::CRITICAL, Category::IO,
        "File not provided"
    );

    std::ifstream in(file_name.c_str(), std::ios::binary);
    if(!in) throwException(
        Severity::CRITICAL, Category::IO, 
        "Could not open " + file_name
    );

    in.seekg(0, std::ios::end);
    const auto size = in.tellg();
    if(size < 0) throwException(
        Severity::CRITICAL, Category::IO,
        "Could not size " + file_name
    );

    std::string contents(static_cast<std::size_t>(size), '\0');
    in.seekg(0, std::ios::beg);
    if(
        !in.read(
            contents.data(), 
            static_cast<std::streamsize>(size)
    )) throwException( Severity::CRITICAL, Category::IO, 
        "Failed to read " + file_name
    );
    return contents;
}

