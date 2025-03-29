#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <Metal/Metal.hpp>
#include "readShaderFile.h"

std::string readShaderFile(const std::string &filePath)
{
  std::cout << "FILE PATH: " << filePath << std::endl;
  std::ifstream file(filePath);
  if (!file.is_open())
  {
    std::cerr << "Failed to open shader file: " << filePath << std::endl;
    return "";
  }
  else
  {
    std::cout << "shader file is OPEN: " << filePath << std::endl;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

//void loadShaderFromFile(MTL::Device *device, const std::string &filePath)
void loadShaderFromFile(MTL::Library* & inLib, MTL::Device *device, const std::string &filePath)
{
  std::string shaderSource = readShaderFile(filePath);
  if (shaderSource.empty())
    return;

 NS::String* source = NS::String::string(shaderSource.c_str(), NS::UTF8StringEncoding);
  //std::cout << "SOURCE: " << nsFilePath << std::endl;
  NS::Error *error = nullptr;
  MTL::CompileOptions* compileOptions = MTL::CompileOptions::alloc()->init();
  MTL::Library *library = device->newLibrary(source, compileOptions, &error);
  //MTL::Library *library = device->newLibrary(source, &error); // doesn't work??
  compileOptions->release();

  if (!library)
  {
    if (error) {
      std::cerr << "Failed to compile Metal library: " << error->localizedDescription()->utf8String() << std::endl;
  } else {
      std::cerr << "Failed to compile Metal library: Unknown error" << std::endl;
  }
  return;
  }
  else
  {
    std::cout << "Shader compiled successfully!" << std::endl;
    inLib = library;
  }
}
