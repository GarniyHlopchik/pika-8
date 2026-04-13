#pragma once

struct DebugVertex{
    float x, y, z;
    Color color;
};

enum class DebugPrimitiveType {
    Lines,
    LineLoop,
    LineStrip,
    TriangleFan
};

struct DebugDrawCommand {
    std::vector<DebugVertex> vertices;
    DebugPrimitiveType type;
};
