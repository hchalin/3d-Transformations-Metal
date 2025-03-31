#include "quad.h"
#include "./shaders/readShaderFile.h"

Quad::Quad(MTL::Device *device) : device(device),
                                  vertexBuffer(nullptr),
                                  indexBuffer(nullptr),
                                  pipelineState(nullptr)
{

  createVertexBuffer();
  createIndexBuffer();
  createRenderPipelineState();
}
// Destructor
Quad::~Quad()
{
  if (device)
  {
    device = nullptr;
  }

  if (vertexBuffer)
  {
    vertexBuffer->release();
    vertexBuffer = nullptr;
  }

  if (indexBuffer)
  {
    indexBuffer->release();
    indexBuffer = nullptr;
  }

  if (pipelineState)
  {
    pipelineState->release();
    pipelineState = nullptr;
  }
}

void Quad::createVertexBuffer()
{
  // Define the vertecies of the quad and assign them to a buffer
  struct Vertex
  {
    float x;
    float y;
    float z;
    float w;
  };

  Vertex vertices[] = {
      {-0.5, -0.5, 0.0, 1.0}, // Bottom Left
      {-0.5, 0.5, 0.0, 1.0},  // Top Left
      {0.5, 0.5, 0.0, 1.0},   // Top Right
      {0.5, -0.5, 0.0, 1.0}}; // Bottom Right

  vertexBuffer = device->newBuffer(vertices, sizeof(vertices), MTL::ResourceStorageModeManaged);

  if (!vertexBuffer)
  {
    throw std::runtime_error("Failed to create vertex buffer");
  }
  else
  {
    std::cout << "Quad Vertex buffer created!" << std::endl;
  }
}

void Quad::createIndexBuffer()
{
  // Create the indices representing the two triangles
  uint16_t indices[] = {
      // First
      1, 3, 0,
      // Second
      1, 2, 3};

  indexBuffer = device->newBuffer(indices, sizeof(indices), MTL::ResourceStorageModeManaged);

  if (!indexBuffer)
  {
    throw std::runtime_error("Failed to create index buffer");
  }
  else
  {
    std::cout << "Quad Index buffer created!" << std::endl;
  }
}

void Quad::createRenderPipelineState()
{

  // verify creation of vertex buffer and index buffer
  if (!vertexBuffer || !indexBuffer)
  {
    throw std::runtime_error("Quad: Missing vertex or index buffer in createRenderPipelineState()");
  }

  // Define shaders
  const std::string shaderPath = "./src/shaders/shaders.metal";
  NS::Error *error{nullptr};
  MTL::Library *library{nullptr};

  try
  {
    loadShaderFromFile(library, device, shaderPath);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error loading quad shader: " << e.what() << std::endl;
  }

  // Check library for creation
  if (!library)
    throw std::runtime_error("Failed to create quad shader library");
  else
    std::cout << "Quad library created" << std::endl;

  // Vertex function
  MTL::Function *vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
  MTL::Function *fragmentFunction = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
  if (!vertexFunction || !fragmentFunction)
    throw std::runtime_error("Failed to create MTL Functions");

  // Create descriptor to describe the pipeline state
  MTL::RenderPipelineDescriptor *pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
  // Set the functions
  pipelineDescriptor->setVertexFunction(vertexFunction);
  pipelineDescriptor->setFragmentFunction(fragmentFunction);

  // Configure color attachment - NOTE: The color attachment represents the output target for the fragment shader.
  MTL::RenderPipelineColorAttachmentDescriptor *colorAttachment = pipelineDescriptor->colorAttachments()->object(0);
  colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
  colorAttachment->setBlendingEnabled(false); // This overwrites the entire color buffer, keep this in mind when rendering fog etc...

  // Create and configure the vertex descriptor
  MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
  vertexDescriptor->layouts()->object(0)->setStride(sizeof(float) * 4);                        // Each vertex is a float4 (x, y, z, w)
  vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat4); // float4 for position
  vertexDescriptor->attributes()->object(0)->setOffset(0);                                     // Position starts at offset 0
  vertexDescriptor->attributes()->object(0)->setBufferIndex(0);                                // Use buffer index 0

  // Attach the vertex descriptor to the pipeline descriptor
  pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
  // Create the pipeline state
  pipelineState = device->newRenderPipelineState(pipelineDescriptor, &error);
  if (!pipelineState)
  {
      throw std::runtime_error("Failed to create render pipeline state: " + std::string(error->localizedDescription()->utf8String()));
  }
  else
  {
      std::cout << "Quad render pipeline state created!" << std::endl;
  }

  // Release temporary resources
  pipelineDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
  library->release();
}

void Quad::encodeRenderCommands(MTL::RenderCommandEncoder *encoder)
{
  if (!pipelineState || !vertexBuffer || !indexBuffer)
  {
      throw std::runtime_error("Quad: Missing pipeline state, vertex buffer, or index buffer in encodeRenderCommands()");
  }

  // Set the pipeline state
  encoder->setRenderPipelineState(pipelineState);

  // Bind the vertex buffer
  encoder->setVertexBuffer(vertexBuffer, 0, 0);

  // Draw the quad using the index buffer
  encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                  6, // Number of indices
                                  MTL::IndexType::IndexTypeUInt16,
                                  indexBuffer,
                                  0); // 
}
