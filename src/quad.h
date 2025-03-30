#pragma once

#include <iostream>
#include <Metal/Metal.hpp>

class Quad {

  public:
    Quad() = default;
    Quad(MTL::Device* device);
    ~Quad();

    void encodeRenderCommands(MTL::RenderCommandEncoder* encoder);


  private:
    //Methods
    void createVertexBuffer();
    void createRenderPipelineState();

    // Members
    MTL::Device* device;
    MTL::Buffer* vertexBuffer;
    MTL::Buffer* indexBuffer;
    MTL::RenderPipelineState* pipelineState;

};
