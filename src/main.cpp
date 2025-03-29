// Prevent GLFW from including OpenGL headers (we're using Metal instead)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Enable private implementation details for Apple's frameworks
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

// Include Apple's framework headers for Metal and related APIs
//#include <Foundation/Foundation.hpp>
//#include <Metal/Metal.hpp>
//#include <QuartzCore/QuartzCore.hpp>
//#include <AppKit/AppKit.hpp>

// Backend
#include "backend/glfw_adaptor.h"

#include "window.h"
#include "renderer.h"

#include <iostream>

int main(){
  // Test glfw
  if (!glfwInit())
  {
      std::cerr << "Failed to initialize GLFW window!" << std::endl;
      return -1;
  }

  try {
    Window window;
    Renderer renderer(window);

    renderer.render();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwTerminate();
  return 0;
}

#ifdef OLD
int main()
{
  if (!glfwInit())
  {
      std::cerr << "Failed to initialize GLFW window!" << std::endl;
      return -1;
  }

  // disable GLFW's default behavior of creating an OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow * glfwWindow = glfwCreateWindow(800, 600, "Hello Metal", nullptr, nullptr);

  MTL::Device * device = MTL::CreateSystemDefaultDevice();

  CA::MetalLayer* metalLayer = CA::MetalLayer::layer()->retain();
  metalLayer->setDevice(device);
  metalLayer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

  NS::Window * window = get_ns_window(glfwWindow, metalLayer)->retain();

  while(!glfwWindowShouldClose(glfwWindow)){
    glfwPollEvents();
  }

  window->release();
  metalLayer->release();

  glfwDestroyWindow(glfwWindow);
  glfwTerminate();
  return 0;
}
#endif
