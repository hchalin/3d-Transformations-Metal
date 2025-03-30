#include "triangle.h"
#include "./shaders/readShaderFile.h"

Triangle::Triangle(MTL::Device *device) : device(device), vertexBuffer(nullptr), pipelineState(nullptr)
{
  createVertexBuffer();
  createRenderPipelineState();
}

Triangle::~Triangle()
{

  if (device)
    device = nullptr;

  if (vertexBuffer){
    vertexBuffer->release();
    vertexBuffer = nullptr;
  }

  if (pipelineState){
    pipelineState->release();
    pipelineState = nullptr;
  }
}

void Triangle::createVertexBuffer()
{
  struct Vertex
  {
    float x;
    float y;
    float z;
    float w;
  };
  // Define vertices
  Vertex vertices[] = {
      {0.0, 0.5, 0.0, 1.0},
      {-0.5, -0.5, 0.0, 1.0},
      {0.5, -0.5, 0.0, 1.0}};

  // create the vertex buffer
  /*
  ResourceStorageModeManaged indicates that the buffer's contents are managed by both the CPU and GPU, allowing the CPU to modify the data and synchronize it with the GPU.
  */
  vertexBuffer = device->newBuffer(vertices, sizeof(vertices), MTL::ResourceStorageModeManaged);

  if (!vertexBuffer)
  {
    std::cerr << "Failed to create vertex buffer" << std::endl;
  }
  else
  {
    std::cout << "Vertex buffer created!" << std::endl;
  }
}

/**
 * This function performs the following steps:
 * 1. Loads the Metal shader library from a specified file path.
 * 2. Retrieves the vertex and fragment shader functions from the library.
 * 3. Configures a render pipeline descriptor with the shader functions and color attachment settings.
 *
 * @note The function uses a helper function `loadShaderFromFile` to load the shader library.
 *       Ensure that the shader file exists at the specified path and is valid.
 */
void Triangle::createRenderPipelineState()
{
  // Uses helper function to load the shaders - using pre-compiled shaders is another approach
  const std::string shaderPath = "./src/shaders/vertex.metal";
  NS::Error *error{nullptr}; // Used to catch errors when creating the library
  MTL::Library *library{nullptr};

  try
  {
    loadShaderFromFile(library, device, shaderPath);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error loading shader: " << e.what() << std::endl;
  }

  // Create library
  if (!library)
  {
    std::cerr << "Failed to load default metal library: " << error->localizedDescription()->utf8String() << std::endl;
    return;
  }
  else
  {
    std::cout << "Library found" << std::endl;
  }

  // Get both vertex and fragment functions
  MTL::Function* vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
  if (!vertexFunction){
    std::cerr << "Vertex function not found" << std::endl;
    return;
  }

  MTL::Function* fragmentFunction = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
  if (!fragmentFunction){
    std::cerr << "Fragment function not found" << std::endl;
    return;
  }

  // Create render pipeline descriptor
  MTL::RenderPipelineDescriptor* pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
  // Set the functions
  pipelineDescriptor->setVertexFunction(vertexFunction);
  pipelineDescriptor->setFragmentFunction(fragmentFunction);

  // Configure color attachment - NOTE: The color attachment represents the output target for the fragment shader.
  MTL::RenderPipelineColorAttachmentDescriptor* colorAttachment = pipelineDescriptor->colorAttachments()->object(0);
  colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
  colorAttachment->setBlendingEnabled(false);                       // This overwrites the entire color buffer, keep this in mind when rendering fog etc...

  // Create and configure the vertex descriptor
 MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
 vertexDescriptor->layouts()->object(0)->setStride(sizeof(float) * 4); // Each vertex is a float4 (x, y, z, w)
 vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat4); // float4 for position
 vertexDescriptor->attributes()->object(0)->setOffset(0); // Position starts at offset 0
 vertexDescriptor->attributes()->object(0)->setBufferIndex(0); // Use buffer index 0
 pipelineDescriptor->setVertexDescriptor(vertexDescriptor);


  // Now create the renderPipelineState with the descriptor
  pipelineState = device->newRenderPipelineState(pipelineDescriptor, &error);

  if (error){
    std::cerr << "ERROR: " << error->localizedDescription()->utf8String() << std::endl;
    return;
  }

  if (!pipelineState)
  {
    std::cerr << "Invalid pipelineState" << std::endl;
    return;
  } else {
    std::cout << "Render pipeline state created" << std::endl;
  }

  // clean up
  pipelineDescriptor->release();
  vertexFunction->release();
  vertexDescriptor->release();
  fragmentFunction->release();
  library->release();

}

void Triangle::encodeRenderCommands(MTL::RenderCommandEncoder *encoder)
{
  /*
      To encode, you need:
      1: A valid encoder
      2: Complete render pipeline state
      3: Compete Vertex buffer
  */
  if (!encoder)
  {
    std::cerr << "Invalid encoder" << std::endl;
    return;
  }
  //encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeLines);
  // set renderpipeline state using encoder
  encoder->setRenderPipelineState(pipelineState);
  // Set vertex buffer
  encoder->setVertexBuffer(vertexBuffer, 0, 0);

  // Draw call
  // encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 0, 3);

  // There has to be a better way to do this....
  encoder->drawPrimitives(static_cast<MTL::PrimitiveType>(MTL::PrimitiveType::PrimitiveTypeTriangle), static_cast<NS::UInteger>(0), static_cast<NS::UInteger>(3));
}
