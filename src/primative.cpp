#include "primative.h"

/*
-------------------------------------------------------------------
  PRIMATIVE  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Primative::Primative(MTL::Device *device) : device(device),
                                            vertexBuffer(nullptr),
                                            pipelineState(nullptr)
{
  initialize();
}

void Primative::initialize()
{

  std::vector<Vertex> vertices = defineVertices();      // ERROR HERE: Dynamic cast??
  createVertexBuffer(vertices);
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

void Primative::encodeRenderCommands(MTL::RenderCommandEncoder *encoder)
{
  if (encoder)
    std::cout << "Ready to encode commands" << std::endl;
}

/*
-------------------------------------------------------------------
    Triangle  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Triangle::Triangle(MTL::Device *device) : Primative(device) {}

// Used base class destructor
std::vector<Vertex> Triangle::defineVertices()
{
  std::cout << "Defining vertices" << std::endl;
  return std::vector<Vertex>{
      {0.0, 0.5, 0.0, 1.0},
      {-0.5, -0.5, 0.0, 1.0},
      {0.5, -0.5, 0.0, 1.0}};
}

/*
-------------------------------------------------------------------
    Quad  ---------------------------------------------------------
-------------------------------------------------------------------
*/
Quad::Quad(MTL::Device *device) : Primative(device) {}

Quad::~Quad(){


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
