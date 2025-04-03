#pragma once

#include <iostream>

#include <Metal/Metal.hpp>
/*
A lot of this code is duplicated for all objects. Consider creating an
abstract base class of an object, and deriving primatives you want
*/

class Triangle {
  public:
    Triangle() = default;
    Triangle(MTL::Device* device);
    ~Triangle();

    void encodeRenderCommands(MTL::RenderCommandEncoder* encoder);

  private:
    void createVertexBuffer();
    void createRenderPipelineState();

    MTL::Device* device;
    MTL::Buffer* vertexBuffer;
    MTL::RenderPipelineState* pipelineState;


};
