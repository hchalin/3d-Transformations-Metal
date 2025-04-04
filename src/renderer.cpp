#include "renderer.h"

//#define TRIANGLE
//#define QUAD
#define CIRCLE
//#define LOG

/**
 * @brief Constructor for the Renderer class.
 *
 * Initializes the Metal device, command queue, and creates the triangle or quad object.
 *
 * @param window Reference to the Window object.
 */
Renderer::Renderer(Window &window) : device(nullptr), commandQueue(nullptr), window(window),
                                     triangle(nullptr),circle(nullptr), previousTime(std::chrono::high_resolution_clock::now()), totalTime(0.0),
                                     lastPrintedSecond(-1), frames(0)
{
  // Get device from the windows metal layer
  device = window.getMetalLayer()->device();
  if (!device)
    throw std::runtime_error("Failed to get Metal Device");

// Create quad
#ifdef QUAD
  quad = new Quad(device);
  // quad = dynamic_cast<Quad*>(new Quad(device));
#endif /* QUAD */

// Create triangle
#ifdef TRIANGLE
  triangle = new Triangle(device);
#endif /* TRIANGLE */
#ifdef CIRCLE
    circle = new Circle(device);
#endif /* CIRCLE */

  // Create the command queue (created from the device)
  commandQueue = device->newCommandQueue()->retain();

  // Render
  render();
}
/**
 * @brief Destructor for the Renderer class.
 *
 * Cleans up allocated resources. Currently commented out due to segfault issues.
 */
Renderer::~Renderer()
{
    // FIXME: There is a segfault here when deallocating?
    /*
  if (triangle)
  {
    delete triangle;
    triangle = nullptr;
  }

  if (quad)
  {
    delete quad;
    quad = nullptr;
  }

  if (commandQueue)
    commandQueue->release();

  if (device)
    device = nullptr;
    */
}
/**
 * @brief Main render loop.
 *
 * Continuously renders frames until the window is closed.
 */
void Renderer::render()
{

  while (!glfwWindowShouldClose(window.getGLFWWindow()))
  {
    glfwPollEvents();
    // #define LOG
#ifdef LOG
    logFPS();
#endif /*LOG*/
    {  // create local scope
      NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
      CA::MetalDrawable *drawable = window.getMetalLayer()->nextDrawable();
      if (!drawable)
      {
        std::cerr << "Drawable is null!" << std::endl;
        pool->release();
        break;
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

      // TEST: Disable depth test
      //renderPass->setDepthAttachment(nullptr);

// Command encoder

      MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);
      float currTime = sin(totalTime);

      encoder->setVertexBytes(&currTime, sizeof(float), 11);

#ifdef QUAD
      if (quad)
      {

        quad->encodeRenderCommands(encoder);
        quad->draw(encoder);
      }

#endif /* QUAD */
#ifdef TRIANGLE
      if (triangle)
      {
        triangle->encodeRenderCommands(encoder); // Needs a RenderCommandEncoder, NOT CommandEncoder
        triangle->draw(encoder);
      }

#endif /* TRIANGLE */
#ifdef CIRCLE
          if (circle)
      {
        circle->encodeRenderCommands(encoder); // Needs a RenderCommandEncoder, NOT CommandEncoder
        circle->draw(encoder);
      }

#endif /* CIRCLE */
      encoder->endEncoding();

      // Present
      commandBuffer->presentDrawable(drawable);
      commandBuffer->commit();

      renderPass->release();

      pool->release();
    }
  }
}

void Renderer::logFPS()
{
  using Clock = std::chrono::high_resolution_clock;

  // Calculate delta time
  auto currentTime = Clock::now();
  std::chrono::duration<double> deltaTime = currentTime - previousTime;
  previousTime = currentTime;

  std::cout << "Delta Time: " << deltaTime.count() << " seconds" << std::endl;

  // Increment frame count
  ++frames;

  // Accumulate total time
  totalTime += deltaTime.count();

  // Print FPS once per second
  int currentSecond = static_cast<int>(totalTime);
  if (currentSecond > lastPrintedSecond)
  {
    std::cout << "Total Time: " << currentSecond << " seconds" << std::endl;
    std::cout << "FPS: " << frames << std::endl;

    // Update the last printed second and reset frame counter
    lastPrintedSecond = currentSecond;
    frames = 0;
  }
}
