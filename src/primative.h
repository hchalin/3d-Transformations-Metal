#pragma once

#include <iostream>

#include <Metal/Metal.hpp>
#include "common/common.h"
#include "common/vec4.h"

class Primative
{
public:
  Primative(MTL::Device *device);
  virtual ~Primative(); // Special case for each deallocation

  void encodeRenderCommands(MTL::RenderCommandEncoder *encoder);
  virtual void draw(MTL::RenderCommandEncoder *encoder) = 0;


protected:
  MTL::Device *device {nullptr};
  MTL::Buffer *vertexBuffer {nullptr};
  MTL::Buffer *indexBuffer {nullptr};
  MTL::Buffer* colorBuffer {nullptr};
  MTL::RenderPipelineState *pipelineState {nullptr};

  void createRenderPipelineState();
  void createVertexBuffer(const std::vector<float4>& vertices);
  void createColorBuffer(const std::vector<float4>& vertices);
  void createIndexBuffer(const std::vector<uint16_t>& indices);
  virtual void createBuffers() = 0;
};

/*
    TRIANGLE
*/
class Triangle : public Primative
{
public:
  Triangle(MTL::Device *device);
  ~Triangle() = default;

  void draw(MTL::RenderCommandEncoder *encoder) override;

protected:
  virtual void createBuffers() override;
};

/*
    QUAD
*/
class Quad : public Primative
{
public:
  Quad(MTL::Device *device);
  ~Quad() override;

  void draw(MTL::RenderCommandEncoder *encoder) override;

private:
  virtual void createBuffers() override;

  MTL::Buffer *indexBuffer;
};
