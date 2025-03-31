#pragma once

#include <iostream>
#include <Metal/Metal.hpp>

class Quad {

  public:
    Quad() = default;
    Quad(MTL::Device* device);
    ~Quad();

    // This would be a good virtual function for dynamic binding
    void encodeRenderCommands(MTL::RenderCommandEncoder* encoder);


  private:
    //Methods
    void createVertexBuffer();
    void createIndexBuffer();
    void createRenderPipelineState();

    // Members
    MTL::Device* device;
    MTL::Buffer* vertexBuffer;
    MTL::Buffer* indexBuffer;
    MTL::RenderPipelineState* pipelineState;

};
