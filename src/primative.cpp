#include "primative.h"
#include "./shaders/readShaderFile.h"

/*
-------------------------------------------------------------------
  PRIMATIVE  ---------------------------------------------------------

  Currently there are 2 types:
    Triangle,
    Quad
-------------------------------------------------------------------
*/
Primative::Primative(MTL::Device *device) : device(device)
{
}

/*
    Destructor
*/
Primative::~Primative()
{

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
void Primative::createVertexBuffer(const std::vector<float4> &vertices)
{
  if (vertices.empty())
    std::runtime_error("No vertices defined");

  vertexBuffer = device->newBuffer(vertices.data(), vertices.size() * sizeof(float4), MTL::ResourceStorageModeManaged);

  if (!vertexBuffer)
    std::runtime_error("Failed to create vertex buffer");
}

/*
    CREATE COLOR BUFFER
*/
void Primative::createColorBuffer(const std::vector<float4> &color)
{
  if (color.empty())
    std::runtime_error("No color defined");

  colorBuffer = device->newBuffer(color.data(), color.size() * sizeof(float4), MTL::ResourceStorageModeManaged);

  if (!colorBuffer)
    std::runtime_error("Failed to create vertex buffer");
}

/*
    CREATE INDEX BUFFER
*/
void Primative::createIndexBuffer(const std::vector<uint16_t> &indices)
{
  indexBuffer = device->newBuffer(indices.data(), indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeManaged);
  if (indexBuffer)
    std::cout << "Index buffer created" << std::endl;
}

/*
  CREATE RENDER PIPELINE STATE
*/
void Primative::createRenderPipelineState()
{
  // Uses helper function to load the shaders - using pre-compiled shaders is another approach
  const std::string fileName = "shaders.metal";
  NS::Error *error{nullptr}; // Used to catch errors when creating the library
  MTL::Library *library{nullptr};

  try
  {
    loadShaderFromFile(library, device, fileName);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error loading shader: " << e.what() << std::endl;
  }

  // Create library
  if (!library)
    throw std::runtime_error("Failed to create triangle shader library");

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

  /*
    OLD - no longer need the vertex descriptor
  // Create and configure the vertex descriptor
  MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

  // Position
  vertexDescriptor->layouts()->object(0)->setStride(sizeof(Vertex));                           // Each vertex is a float4 (x, y, z, w)
  vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat4); // float4 for position
  vertexDescriptor->attributes()->object(0)->setOffset(offsetof(Vertex, position));            // Position starts at offset 0
  vertexDescriptor->attributes()->object(0)->setBufferIndex(0);                                // Use buffer index 0

  // Set color attribute
  vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat4); // float4 for position
  vertexDescriptor->attributes()->object(1)->setOffset(offsetof(Vertex, color));
  vertexDescriptor->attributes()->object(1)->setBufferIndex(0); // Use buffer index 0

  pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
  */

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

  // clean up
  pipelineDescriptor->release();
  // vertexDescriptor->release();
  vertexFunction->release();
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

  if (!encoder)
  {
    std::cerr << "Invalid encoder" << std::endl;
    return;
  }
  // encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeLines);
  //  set renderpipeline state using encoder
  encoder->setRenderPipelineState(pipelineState);

  // Set vertex buffer
  encoder->setVertexBuffer(vertexBuffer, 0, 0); // Set vertexBuffer to buffer(0)
  encoder->setVertexBuffer(colorBuffer, 0, 1);  // Set colorBuffer to buffer(1)
  // Index buffer is set when calling drawIndexedPrimatives()
}

/*
-------------------------------------------------------------------
    Triangle  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Triangle::Triangle(MTL::Device *device) : Primative(device)
{
  createBuffers();
  Primative::createRenderPipelineState();
}

/*
      Draw ------
*/
void Triangle::draw(MTL::RenderCommandEncoder *encoder)
{


  encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                 3, // Number of indices
                                 MTL::IndexType::IndexTypeUInt16,
                                 indexBuffer,
                                 0); //
}

void Triangle::createBuffers()
{
  // Positions
  std::vector<float4> positions = {
      {0.0, 0.5, 0.0, 1.0},
      {-0.5, -0.5, 0.0, 1.0},
      {0.5, -0.5, 0.0, 1.0}};
  Primative::createVertexBuffer(positions);

  // Colors
  std::vector<float4> color = {
      {1.0, 0.0, 0.0, 1.0},
      {1.0, 0.0, 0.0, 1.0},
      {1.0, 0.0, 0.0, 1.0}};
  Primative::createColorBuffer(color);

  // Indexing
  std::vector<uint16_t> indices = {0, 1, 2};
  Primative::createIndexBuffer(indices);

}

/*
-------------------------------------------------------------------
    Quad  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Quad::Quad(MTL::Device *device) : Primative(device), indexBuffer(nullptr)
{
  createBuffers();
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

void Quad::createBuffers()
{
  // Positions
  std::vector<float4> positions = {
      {-0.5, 0.5, 0.0, 1.0}, // Top Left
      {0.5, 0.5, 0.0, 1.0},  // Top Right
      {0.5, -0.5, 0.0, 1.0}, // Bottom Right
      {-0.5, -0.5, 0.0, 1.0} // Bottom Left
  };
  Primative::createVertexBuffer(positions);
  if (!vertexBuffer)
    throw std::runtime_error("Vertex buffer failed to create");

  // Colors
  std::vector<float4> color = {
      {0.0, 0.0, 1.0, 1.0},
      {0.0, 0.0, 1.0, 1.0},
      {0.0, 0.0, 1.0, 1.0},
      {0.0, 0.0, 1.0, 1.0}};

  Primative::createColorBuffer(color);
  if (!colorBuffer)
    throw std::runtime_error("Color buffer failed to create");

  // Indexing
  std::vector<uint16_t> indices = {
      // First tringle
      0, 2, 3,
      // Second triangle
      0, 1, 2};
  //Primative::createIndexBuffer(indices);
  indexBuffer = device->newBuffer(indices.data(), indices.size() * sizeof(float4), MTL::ResourceStorageModeManaged);
  if (!indexBuffer)
    throw std::runtime_error("Index buffer failed to create");

  // test
  std::cout << "SUCCESS in creating Quad buffers" << std::endl;
}

void Quad::draw(MTL::RenderCommandEncoder *encoder)
{

  if (!indexBuffer)
    throw std::runtime_error("Index buffer failed to create");

  // Draw the quad using the index buffer
  encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                 6, // Number of indices
                                 MTL::IndexType::IndexTypeUInt16,
                                 indexBuffer,
                                 0); //
}

//-------------------------------------------------------------------
//    Circle  ---------------------------------------------------------
//-------------------------------------------------------------------

Circle::Circle(MTL::Device *device): Primative(device) {
    // Create the vertex buffer for the circle
    createBuffers();
    Primative::createRenderPipelineState();
}
/*
    Destructor
*/
Circle::~Circle()
{


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

void Circle::createBuffers() {
   /*
    * Position
    */
   std::vector<float4> positions;
   unsigned int vertexCount {100};
   float angle = (2 * M_PI) / vertexCount;
   positions.push_back({0.0, 0.0, 0.0, 1.0});
   for(int i {1}; i <= vertexCount; ++i){
       float x = radius * cos(i * angle);
       float y = radius * sin(i * angle);
       positions.emplace_back(x, y, 0.0, 1.0);
   }

   Primative::createVertexBuffer(positions);
   if (!vertexBuffer) {
       throw std::runtime_error("Failed to create circles vertexBuffer");
   }

   /*
    * Color
    */
   std::vector<float4> color;
   for (int i {0}; i <= vertexCount; ++i){
       color.emplace_back(0.4, 0.2, 0.3, 1.0);
   }
   Primative::createColorBuffer(color);
    if (!colorBuffer)
        throw std::runtime_error("Color buffer failed to create");

    /*
     * Indices
     */
    std::vector<uint16_t> indices;
    const uint16_t centerIdx = 0;
    for (uint16_t i = 1; i <= vertexCount; ++i) {
        // Create two triangles for each segment of the circle
        indices.push_back(centerIdx); // Center vertex
        indices.push_back(i);
        if (i == vertexCount) {
            indices.push_back(1); // Connect back to the first vertex
        } else {
            indices.push_back(i + 1);
        }
    }

    // Add the last triangle to close the circle
    for (uint16_t i = 1; i < vertexCount; ++i) {
        indices.push_back(0); // Center vertex
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    indexBuffer = device->newBuffer(indices.data(), indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeManaged );
    if (!indexBuffer)
        throw std::runtime_error("Index buffer failed to create");
}

void Circle::draw(MTL::RenderCommandEncoder *encoder) {
    // This is the draw call
    std::cout << "Drawing circle" << std::endl;

    encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                   indexBuffer->length() / sizeof(uint16_t), // Number of indices
                                   MTL::IndexType::IndexTypeUInt16,
                                   indexBuffer,
                                   0); //
}
