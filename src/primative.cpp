#include "primative.h"
#include "./shaders/readShaderFile.h"

/*
-------------------------------------------------------------------
  PRIMATIVE  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Primative::Primative(MTL::Device *device) : device(device),
                                            vertexBuffer(nullptr),
                                            indexBuffer(nullptr),
                                            pipelineState(nullptr)
{
  // initialize();
}

void Primative::initialize()
{
}

/*
    Destructor
*/
Primative::~Primative()
{
  if (device)
    device = nullptr;

  if (vertexBuffer)
  {
    vertexBuffer->release();
    vertexBuffer = nullptr;
  }

  if (pipelineState)
  {
    pipelineState->release();
    pipelineState = nullptr;
  }
}

/*
    CREATE VERTEX BUFFER
*/
void Primative::createVertexBuffer(const std::vector<Vertex> vertices)
{
  if (vertices.empty())
    std::runtime_error("No vertices defined");

  vertexBuffer = device->newBuffer(vertices.data(), vertices.size() * sizeof(Vertex), MTL::ResourceStorageModeManaged);

  if (!vertexBuffer)
    std::runtime_error("Failed to create vertex buffer");
  else
    std::cout << "BUFFER CREATED  W/ LENGTH: " << vertexBuffer->length() << std::endl;
}

/*
  CREATE RENDER PIPELINE STATE
*/
void Primative::createRenderPipelineState()
{
  // Uses helper function to load the shaders - using pre-compiled shaders is another approach
  const std::string shaderPath = "./src/shaders/shaders.metal";
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
    throw std::runtime_error("Failed to create triangle shader library");
  else
    std::cout << "Library created" << std::endl;

  // Get both vertex and fragment functions
  MTL::Function *vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
  if (!vertexFunction)
  {
    std::cerr << "Vertex function not found" << std::endl;
    return;
  }

  MTL::Function *fragmentFunction = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
  if (!fragmentFunction)
  {
    std::cerr << "Fragment function not found" << std::endl;
    return;
  }

  // Create render pipeline descriptor
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
  pipelineDescriptor->setVertexDescriptor(vertexDescriptor);

  // Now create the renderPipelineState with the descriptor
  pipelineState = device->newRenderPipelineState(pipelineDescriptor, &error);

  if (error)
  {
    std::cerr << "ERROR: " << error->localizedDescription()->utf8String() << std::endl;
    return;
  }

  if (!pipelineState)
  {
    std::cerr << "Invalid pipelineState" << std::endl;
    return;
  }
  else
  {
    std::cout << "Render pipeline state created" << std::endl;
  }

  // clean up
  pipelineDescriptor->release();
  vertexFunction->release();
  vertexDescriptor->release();
  fragmentFunction->release();
  library->release();
}

/*
    ENCODE RENDER COMMANDS
*/
void Primative::encodeRenderCommands(MTL::RenderCommandEncoder *encoder)
{

  /*
      To encode, you need:
      1: A valid encoder
      2: Complete render pipeline state
      3: Compete Vertex buffer
  */
  if (!pipelineState)
    std::runtime_error("No Pipeline State");
  if (!vertexBuffer)
    std::runtime_error("No Vertex Buffer");

  std::cout << "READY TO ENCODE" << std::endl;
  if (!encoder)
  {
    std::cerr << "Invalid encoder" << std::endl;
    return;
  }
  // encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeLines);
  //  set renderpipeline state using encoder
  encoder->setRenderPipelineState(pipelineState);
  // Set vertex buffer
  encoder->setVertexBuffer(vertexBuffer, 0, 0);

}

/*
-------------------------------------------------------------------
    Triangle  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Triangle::Triangle(MTL::Device *device) : Primative(device)
{
  std::vector<Vertex> vertices = defineVertices(); // ERROR HERE: Dynamic cast??
  Primative::createVertexBuffer(vertices);
  Primative::createRenderPipelineState();
}

// Used base class destructor
std::vector<Vertex> Triangle::defineVertices()
{

  std::cout << "Defining verts" << std::endl;
  return std::vector<Vertex>{
      {0.0, 0.5, 0.0, 1.0},
      {-0.5, -0.5, 0.0, 1.0},
      {0.5, -0.5, 0.0, 1.0}};
}

void Triangle::draw(MTL::RenderCommandEncoder *encoder)
{

  encoder->drawPrimitives(static_cast<MTL::PrimitiveType>(MTL::PrimitiveType::PrimitiveTypeTriangle), static_cast<NS::UInteger>(0), static_cast<NS::UInteger>(3));
}

/*
-------------------------------------------------------------------
    Quad  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Quad::Quad(MTL::Device *device) : Primative(device), indexBuffer(nullptr)
{
  std::vector<Vertex> vertices = defineVertices(); // ERROR HERE: Dynamic cast??
  Primative::createVertexBuffer(vertices);
  createIndexBuffer();
  Primative::createRenderPipelineState();
}

Quad::~Quad()
{

  if (indexBuffer)
  {
    indexBuffer->release();
    indexBuffer = nullptr;
  }
}

std::vector<Vertex> Quad::defineVertices()
{
  return std::vector<Vertex>{
      {-0.5, -0.5, 0.0, 1.0}, // Bottom Left
      {-0.5, 0.5, 0.0, 1.0},  // Top Left
      {0.5, 0.5, 0.0, 1.0},   // Top Right
      {0.5, -0.5, 0.0, 1.0}}; // Bottom Right
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

void Quad::draw(MTL::RenderCommandEncoder *encoder)
{

  // Draw the quad using the index buffer
  encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                 6, // Number of indices
                                 MTL::IndexType::IndexTypeUInt16,
                                 indexBuffer,
                                 0); //
}
