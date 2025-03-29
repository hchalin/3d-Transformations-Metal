#include "renderer.h"

Renderer::Renderer(Window &window) : device(nullptr), commandQueue(nullptr), window(window),
triangle(nullptr)
{
  // Get device from the windows metal layer
  device = window.getMetalLayer()->device();
  if (!device)
    throw std::runtime_error("Failed to get Metal Device");

  // Create triangle
  triangle = new Triangle(device);

  // Create the command queue (created from the device)
  commandQueue = device->newCommandQueue()->retain();
}

Renderer::~Renderer()
{
  delete triangle;
  triangle = nullptr;

  if (commandQueue)
    commandQueue->release();

  if (device)
    device = nullptr;
}

void Renderer::render()
{



  while (!glfwWindowShouldClose(window.getGLFWWindow()))
  {
    glfwPollEvents();
        #define LOG
        #ifdef LOG
        logFPS();
        #endif /*LOG*/
    { // create local scope
      NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
      CA::MetalDrawable *drawable = window.getMetalLayer()->nextDrawable();
      if (!drawable)
      {
        std::cerr << "Drawable is null!" << std::endl;
        return;
      }

      // Create command buffer per frame
      MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();

      // Create render pass descriptor
      MTL::RenderPassDescriptor *renderPass = MTL::RenderPassDescriptor::alloc()->init();
      MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPass->colorAttachments()->object(0);
      colorAttachment->setTexture(drawable->texture());
      colorAttachment->setLoadAction(MTL::LoadActionClear);
      colorAttachment->setClearColor(MTL::ClearColor(4.0, 2.0, 5.0, 1.0));
      colorAttachment->setStoreAction(MTL::StoreActionStore);

      // Command encoder
      #ifdef STATIC_CAST_ENCODER
      // Command encoder is a generic encoder for multiple uses
      MTL::CommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);

      // Encode commands for the triangle
      triangle->encodeRenderCommands(static_cast<MTL::RenderCommandEncoder *>(encoder));
      #endif /* STATIC_CAST_ENCODER */

      MTL::RenderCommandEncoder* encoder = commandBuffer->renderCommandEncoder(renderPass);
      triangle->encodeRenderCommands(encoder);    // Needs a RenderCommandEncoder, NOT CommandEncoder

      encoder->endEncoding();

      // Present
      commandBuffer->presentDrawable(drawable);
      commandBuffer->commit();

      renderPass->release();

      pool->release();
    }
  }
}

void Renderer::logFPS(){
  using Clock = std::chrono::high_resolution_clock;
  auto previousTime = Clock::now(); // Store the starting time
  double totalTime = 0.0;           // Total elapsed time in seconds
  int lastPrintedSecond = -1;       // Track the last second that was printed
  int frames {0};
        // Calculate delta time
        auto currentTime = Clock::now();
        std::chrono::duration<double> deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        // increment frame
        ++frames;

        // Accumulate total time
        totalTime += deltaTime.count();

        // Print total time only once per second
        int currentSecond = static_cast<int>(totalTime);
        if (currentSecond > lastPrintedSecond) {
            std::cout << "Total Time: " << currentSecond << " seconds" << std::endl;
            std::cout << "FPS: " << frames  << std::endl;
            lastPrintedSecond = currentSecond; // update the last printed second
            frames = 0;
        }
}
