#include "debug.h"

static Debug* singleton_instance = nullptr;

Debug::Debug() : VAO(0), VBO(0), shader(0) {
}

Debug::~Debug() { 
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

Debug& Debug::get_instance() {
    if (singleton_instance == nullptr) {
        singleton_instance = new Debug();
    }
    return *singleton_instance;
}


void Debug::init(unsigned int p_shader) {
    Debug& debug = get_instance();
    debug.shader = p_shader;
    
    // Initialize debug VAO/VBO
    glGenVertexArrays(1, &debug.VAO);
    glGenBuffers(1, &debug.VBO);
    
    glBindVertexArray(debug.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, debug.VBO);
    
    // Allocate buffer for debug vertices (max 10k vertices per frame)
    glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
    
    // Vertex attributes: position (x, y, z) + color (r, g, b, a)
    // Location 0: position (3 floats)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)0);
    
    // Location 1: color (4 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)(sizeof(float) * 3));
    
    glBindVertexArray(0);

    auto [width, height] = GFX::get_screen_size();
    set_projection(width, height);
}

void Debug::begin_frame() {
    get_instance().frameCommands.clear();
}

void Debug::end_frame() {
    // Commands are flushed separately via flush()
}

void Debug::set_projection(int width, int height) {
    Debug& debug = get_instance();
    if (debug.shader == 0) {
        return;
    }
    
    float proj[16] = {
        2.0f/width, 0, 0, -1,
        0, -2.0f/height, 0, 1,
        0, 0, -1, 0,
        0, 0, 0, 1
    };
    glUseProgram(debug.shader);
    unsigned int location = glGetUniformLocation(debug.shader, "uProjection");
    glUniformMatrix4fv(location, 1, GL_TRUE, proj);
}

void Debug::draw_line(float x1, float y1, float x2, float y2, Color color) {
    std::vector<DebugVertex> vertices = {
        {x1, y1, 0.0f, color},
        {x2, y2, 0.0f, color}
    };
    draw_debug_shape(vertices, DebugPrimitiveType::Lines);
}

void Debug::draw_rect(float x, float y, float width, float height, Color color) {
    x -= width * 0.5f;
    y -= height * 0.5f;
    // Draw as line loop (unfilled rectangle outline)
    std::vector<DebugVertex> vertices = {
        {x, y, 0.0f, color},
        {x + width, y, 0.0f, color},
        {x + width, y + height, 0.0f, color},
        {x, y + height, 0.0f, color}
    };
    draw_debug_shape(vertices, DebugPrimitiveType::LineLoop);
}

void Debug::draw_circle(float centerX, float centerY, float radius, Color color) {
    const int segments = 36;
    std::vector<DebugVertex> vertices;
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * 3.14159265f * i) / segments;
        float x = centerX + radius * std::cos(angle);
        float y = centerY + radius * std::sin(angle);
        vertices.push_back({x, y, 0.0f, color});
    }
    draw_debug_shape(vertices, DebugPrimitiveType::LineLoop);
}

void Debug::draw_polygon(const std::vector<std::pair<float, float>>& vertices, Color color) {
    std::vector<DebugVertex> debugVertices;
    for (const auto& vertex : vertices) {
        debugVertices.push_back({vertex.first, vertex.second, 0.0f, color});
    }
    draw_debug_shape(debugVertices, DebugPrimitiveType::LineLoop);
}

void Debug::draw_debug_shape(const std::vector<DebugVertex>& vertices, DebugPrimitiveType type) {
    Debug& debug = get_instance();
    DebugDrawCommand command;
    command.vertices = vertices;
    command.type = type;
    debug.frameCommands.push_back(command);
}

void Debug::flush(unsigned int p_shader) {
    Debug& debug = get_instance();
    
    if (debug.frameCommands.empty() || debug.VAO == 0) {
        return;
    }
    
    glUseProgram(p_shader != 0 ? p_shader : debug.shader);
    glBindVertexArray(debug.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, debug.VBO);
    
    size_t totalVertices = 0;
    
    // Accumulate all vertices from all commands
    std::vector<DebugVertex> allVertices;
    for (const auto& cmd : debug.frameCommands) {
        for (const auto& v : cmd.vertices) {
            allVertices.push_back(v);
        }
    }
    
    // Upload all vertices at once
    if (!allVertices.empty()) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, allVertices.size() * sizeof(DebugVertex), allVertices.data());
    }
    
    // Render grouped by primitive type
    size_t offset = 0;
    for (const auto& cmd : debug.frameCommands) {
        GLenum glPrimitiveType;
        switch (cmd.type) {
            case DebugPrimitiveType::Lines:
                glPrimitiveType = GL_LINES;
                break;
            case DebugPrimitiveType::LineLoop:
                glPrimitiveType = GL_LINE_LOOP;
                break;
            case DebugPrimitiveType::LineStrip:
                glPrimitiveType = GL_LINE_STRIP;
                break;
            case DebugPrimitiveType::TriangleFan:
                glPrimitiveType = GL_TRIANGLE_FAN;
                break;
            default:
                glPrimitiveType = GL_LINE_LOOP;
        }
        
        glDrawArrays(glPrimitiveType, offset, cmd.vertices.size());
        offset += cmd.vertices.size();
    }
    
    glBindVertexArray(0);
}