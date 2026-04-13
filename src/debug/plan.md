## Plan: Debug GPU Overlay Workflow

TL;DR: Build a GPU-oriented debug renderer separate from the Node system, with a frame-local command buffer for hitboxes, labels, lines, and future overlays. Use a new debug shader/mesh pair and expose simple APIs so game logic can submit debug primitives directly.

Steps
1. Define the debug renderer core.
   - Add a new `DebugRenderer` class in `src/debug/debug_renderer.h` and `src/debug/debug_renderer.cpp`.
   - Use a dedicated VAO/VBO for debug primitives and `GL_DYNAMIC_DRAW` upload.
   - Support line and rectangle primitives at first, then text labels as a second phase.
2. Create debug command data structures.
   - Define `DebugVertex { float x, y, float r, g, b, a; }`.
   - Define `DebugPrimitiveType { Line, Rect, FilledRect, Text }`.
   - Define `DebugDrawCommand` objects with positions, color, optional label, and ID metadata.
3. Implement command submission API.
   - In `src/debug/debug.h` expose functions like `Debug::line(...)`, `Debug::rect(...)`, `Debug::label(...)`.
   - Add a `Debug::begin_frame()` and `Debug::end_frame()` lifecycle.
   - Ensure the API is independent of `Node` / scene tree traversal.
4. Integrate into graphics update flow.
   - Modify `src/gfx/gfx.h` and `src/gfx/gfx.cpp` so `GFX::update()` invokes debug flush.
   - Initialize `DebugRenderer` after the main GL context is ready and before `spritemesh.init()` if needed.
   - Use `DebugRenderer::flush()` around the end of the frame so GPU drawing happens once per frame.
5. Add entity annotation helpers.
   - Use existing `Node::id` from `src/scene_tree/node.h` as a numeric label source.
   - Add higher-level helpers: `draw_hitbox(Node& node, Rect hitbox, Color color)`, `draw_enemy_id(Node& node, int id, Color color)`.
6. Build a future-proof category/filter system.
   - Add debug categories like `Physics`, `AI`, `Collision`, `Pathfinding`.
   - Allow toggling categories in one global `DebugSettings` object.

Relevant files
- `src/debug/debug.h` — expose debug API and lifecycle.
- `src/debug/debug.cpp` — forward API into the renderer.
- `src/debug/debug_renderer.h` — new renderer declaration.
- `src/debug/debug_renderer.cpp` — new renderer implementation.
- `src/gfx/gfx.h` — add debug member or reference.
- `src/gfx/gfx.cpp` — initialize and flush debug rendering.
- `src/scene_tree/node.h` — use existing node IDs as debug labels.

Verification
1. Confirm `GFX::update()` still calls `spritemesh.flush(shader)` and add debug flush without breaking frame order.
2. Render a simple test overlay: one red rectangle and one green line, independent of any `Node` draw call.
3. Create a sample “enemy debug” use case that draws a hitbox and `id` label from a `Node`.
4. Check that no `Node` subclasses need to change for basic debug rendering.

Decisions
- Keep debug rendering separate from current node/sprite pipeline.
- Start with line/rect primitives and add text overlays later.
- Use one GPU-based renderer rather than immediate CPU drawing.
- Use `DebugRenderer` for future expansion to natively support hitboxes, IDs, sensor radii, and path overlays.

Further Considerations
1. If text labels are needed immediately, reuse the existing font system or add a simple bitmap-font debug shader.
2. If the engine later supports multiple render layers, place debug overlays after main scene draw but before buffer swap.
3. For performance, keep commands per frame small and use `glBufferSubData` rather than re-creating buffers.
