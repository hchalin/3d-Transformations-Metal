#pragma once

// Position structure
struct Position
{
    float x, y, z, w;
};

// Color structure
struct Color
{
    float r, g, b, a;
};

// Vertex structure
struct Vertex
{
    Position position;
    Color color;
};
