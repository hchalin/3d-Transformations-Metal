#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <Metal/Metal.hpp>
#include "readShaderFile.h"

std::string readShaderFile(const std::string &targetFileName)
{

    std::string fileName;
    {
        namespace fs = std::filesystem;
        using namespace std;
        fs::path startPath = fs::current_path().parent_path();
        std::cout << "Start path " << startPath << std::endl;
        std::cout << "Searching for " << targetFileName << std::endl;

        for (auto &p : fs::recursive_directory_iterator(startPath))
        {
            if (p.path().filename() == targetFileName)
            {
                fileName = p.path().string();
                std::cout << "Target file: " << targetFileName << " FOUND!" << std::endl;
                std::cout << "File with source path: " << targetFileName << " FOUND!" << std::endl;
                break;
            }
        }
    }

    std::cout << "Path " << fileName << std::endl;
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + fileName);
    }
    else
    {
        std::cout << fileName << " is open!!!----" << std::endl;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// void loadShaderFromFile(MTL::Device *device, const std::string &filePath)
void loadShaderFromFile(MTL::Library *&inLib, MTL::Device *device, const std::string &fileName)
{
    std::string shaderSource = readShaderFile(fileName);
    std::cout << "Shader source:  " << shaderSource << std::endl;

    if (shaderSource.empty())
        std::runtime_error("Shaders file is empty");

    NS::String *source = NS::String::string(shaderSource.c_str(), NS::UTF8StringEncoding);
    // std::cout << "SOURCE: " << nsFilePath << std::endl;
    NS::Error *error = nullptr;
    MTL::CompileOptions *compileOptions = MTL::CompileOptions::alloc()->init();
    MTL::Library *library = device->newLibrary(source, compileOptions, &error);
    // MTL::Library *library = device->newLibrary(source, &error); // doesn't work??
    compileOptions->release();

    if (!library)
    {
        if (error)
        {
            std::cerr << "Failed to compile Metal library: " << error->localizedDescription()->utf8String() << std::endl;
        }
        else
        {
            std::cerr << "Failed to compile Metal library: Unknown error" << std::endl;
        }
        return;
    }
    else
    {
        inLib = library;
    }
}
