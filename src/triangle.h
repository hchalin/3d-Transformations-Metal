#pragma once

#include <iostream>

#include <Metal/Metal.hpp>

class Triangle {
  public:
    Triangle();
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
