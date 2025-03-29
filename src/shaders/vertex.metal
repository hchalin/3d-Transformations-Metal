#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float4 position [[attribute(0)]];
};

struct VertexOut {
    float4 position [[position]]; // Position in clip space
    float4 color;                 // Color to pass to the fragment shader
};

vertex VertexOut vertex_main(VertexIn in [[stage_in]]) {
    VertexOut out;
    out.position = in.position; // Pass position to clip space

    // Compute color based on position
    out.color = float4((in.position.x + 1.0) * 0.5, // Map x from [-1, 1] to [0, 1]
                       (in.position.y + 1.0) * 0.5, // Map y from [-1, 1] to [0, 1]
                       1.0 - ((in.position.x + in.position.y + 2.0) * 0.25), // Some variation for blue
                       1.0); // Alpha is 1.0 (fully opaque)

    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    return in.color; // Use the interpolated color
}
