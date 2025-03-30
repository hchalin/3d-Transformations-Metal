#include "quad.h"
Quad::Quad(MTL::Device* device):
device(device),
vertexBuffer(nullptr),
indexBuffer(nullptr),
pipelineState(nullptr)
{

  createVertexBuffer();

}
// Destructor
Quad::~Quad(){
 if (device){
  device = nullptr;
 }

 if (vertexBuffer)
  vertexBuffer->release();

  if (indexBuffer)
    indexBuffer->release();

  if (pipelineState)
    pipelineState->release();
}

void Quad::createVertexBuffer(){
  // Define the vertecies of the quad and assign them to a buffer
  struct Vertex
  {
    float x;
    float y;
    float z;
    float w;
  };

  Vertex vertices[] = {
    {-0.5, -0.5, 0.0, 1.0},
    {-0.5, 0.5, 0.0, 1.0},
    {0.5, 0.5, 0.0, 1.0},
    {0.5, -0.5, 0.0, 1.0}
  };

  vertexBuffer = device->newBuffer(vertices, sizeof(vertices), MTL::ResourceStorageModeManaged);


  if (!vertexBuffer)
  {
    std::cerr << "Failed to create vertex buffer" << std::endl;
  }
  else
  {
    std::cout << "Quad Vertex buffer created!" << std::endl;
  }
}
