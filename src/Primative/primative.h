#pragma once

#include <iostream>
#include <math.h>
#include <cstdlib>

#include <Metal/Metal.hpp>
#include "../common/common.h"
#include "../common/vec4.h"


class Primitive {
public:
    explicit Primitive(MTL::Device *device);

    virtual ~Primitive() = 0; // Special case for each deallocation

    void encodeRenderCommands(MTL::RenderCommandEncoder *encoder) const;

    virtual void draw(MTL::RenderCommandEncoder *encoder) = 0;

protected:
    MTL::Device *device{nullptr};
    MTL::Buffer *vertexBuffer{nullptr};
    MTL::Buffer *indexBuffer{nullptr};
    MTL::Buffer *colorBuffer{nullptr};
    MTL::RenderPipelineState *pipelineState{nullptr};

    void createRenderPipelineState();

    void createVertexBuffer(const std::vector<float4> &vertices);

    void createColorBuffer(const std::vector<float4> &vertices);

    void createIndexBuffer(const std::vector<uint16_t> &indices);

    virtual void createBuffers() = 0;
};

/*
    TRIANGLE
*/
class Triangle final : public Primitive {
public:
    explicit Triangle(MTL::Device *device);
    Triangle(MTL::Device *device, const std::vector<float4> & vertices, const std::vector<float4> & color);
    ~Triangle() override;

    void draw(MTL::RenderCommandEncoder *encoder) override;

protected:
    void createBuffers() override;
};

/*
    QUAD
*/
class Quad final : public Primitive {
public:
    explicit Quad(MTL::Device *device);

    ~Quad() override;

    void draw(MTL::RenderCommandEncoder *encoder) override;

private:
    void createBuffers() override;

    MTL::Buffer *indexBuffer;
};

/*
 *    CIRCLE
 */

class Circle final : public Primitive {
public:
    explicit Circle(MTL::Device *device);

    ~Circle() override;

    void draw(MTL::RenderCommandEncoder *encoder) override;

private:
    // Members
    float radius{0.5};

    // Methods
    void createBuffers() override;
};
