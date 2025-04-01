#pragma once

#include <iostream>

#include <Metal/Metal.hpp>

// TODO: Add this to a common.h file
struct Vertex
{
  float x;
  float y;
  float z;
  float w;
};

class Primative
{
public:
  Primative(MTL::Device *device);
  virtual ~Primative(); // Special case for each deallocation

  void encodeRenderCommands(MTL::RenderCommandEncoder *encoder);
  virtual void draw(MTL::RenderCommandEncoder* encoder) = 0;

private:
  void initialize();


protected:
  MTL::Device *device;
  MTL::Buffer *vertexBuffer;
  MTL::Buffer *indexBuffer;
  MTL::RenderPipelineState *pipelineState;

  void createRenderPipelineState();
  void createVertexBuffer(const std::vector<Vertex> vertices);
  virtual std::vector<Vertex> defineVertices() = 0;
};

/*
    TRIANGLE
*/
class Triangle : public Primative
{
public:
  Triangle(MTL::Device *device);
  ~Triangle() = default;

  void draw(MTL::RenderCommandEncoder* encoder) override;

protected:
  virtual std::vector<Vertex> defineVertices() override;
};

/*
    QUAD
*/
class Quad : public Primative
{
public:
  Quad(MTL::Device *device);
  ~Quad() override;

  void draw(MTL::RenderCommandEncoder* encoder) override;

private:
  virtual std::vector<Vertex> defineVertices() override;
  void createIndexBuffer();

  MTL::Buffer *indexBuffer;
};
