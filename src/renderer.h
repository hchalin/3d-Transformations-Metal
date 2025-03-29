#pragma once
// Include Apple's framework headers for Metal and related APIs
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <AppKit/AppKit.hpp>

#include "window.h"
#include "triangle.h"

#include <iostream>

class Renderer
{
public:
  Renderer(Window &window);
  ~Renderer();

  // Getter
  MTL::Device *getDevice();

  // Render method
  void render();

private:
  void logFPS();
  MTL::Device* device;
  MTL::CommandQueue* commandQueue;
  Window& window;

  Triangle* triangle;

};
