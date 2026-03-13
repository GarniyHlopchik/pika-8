To generalize the tutorial, we will shift the focus from a specific "player" sprite to the concept of **Texture Manipulation and Clipping**. This version explains how the `GFX.spr` function acts as a window into an image asset, allowing you to render specific regions of a texture.

Here is the generalized version of **2-first_sprite.md**:

---

## 1. Loading Assets

To display graphics in Pika-8, you must first load an image file into memory as a **Texture**. This should be done inside the `_init()` function to ensure the file is only read once when the game starts.

```lua
function _init()
    -- Load an image file into a variable
    my_texture = GFX.load("sprite_sheet.png")
end

```

## 2. The Rendering Function

The primary tool for drawing textures is `GFX.spr()`. While it can draw a full image, its true power lies in its ability to "cut" or "clip" specific sections of a larger sprite sheet.

### Function Signature

`GFX.spr(texture, x, y, w, h, x1, x2, y1, y2)`

| Parameter | Description |
| --- | --- |
| **texture** | The variable containing your loaded image. |
| **x, y** | The position on the screen where the sprite will appear. |
| **w, h** | The width and height of the sprite as it appears on screen. |
| **x1, x2** | The horizontal start and end points of the "cut" from the source image. |
| **y1, y2** | The vertical start and end points of the "cut" from the source image. |

## 3. Understanding the "Cut" System

Instead of using a width/height for the source selection, the engine uses **coordinate boundaries**. Think of `x1, x2, y1, y2` as markers that define a rectangular area within your image file.

### Example: Selecting a Sub-Region

Imagine you have a texture that is 16 pixels wide and 8 pixels high. If you only want to display the middle section, you define the start and end points of the horizontal and vertical axes.

In the diagram below, `*` represents unused pixels and `#` represents the area being rendered:

```text
Horizontal (x): 0 1 2 3 4 5 6 7 8 9 a b c d e f
Vertical (y) 0: * * * * * * * * * * * * * * * * 1: * * * * * * * * * * * * * * * * 2: * * * [ # # # # # # # # ] * * * <-- y1 (start)
             3: * * * [ # # # # # # # # ] * * *
             4: * * * [ # # # # # # # # ] * * *
             5: * * * [ # # # # # # # # ] * * * <-- y2 (end)
             6: * * * * * * * * * * * * * * * * 7: * * * * * * * * * * * * * * * * ^                 ^
                      x1 (3)            x2 (b)

```

To render exactly the `#` area at position (50, 50) on your screen, you would use:

```lua
function _update(delta)
    GFX.cls()
    -- GFX.spr(texture, x, y, w, h, x1, x2, y1, y2)
    GFX.spr(my_texture, 50, 50, 48, 48, 3, 11, 2, 5)
end

```

## 4. Why use this system?

This boundary-based clipping is essential for:

1. **Sprite Sheets:** Storing all your frames (walk, jump, attack) in a single file and "cutting" out the one you need.
2. **Atlases:** Packing multiple different objects (trees, rocks, enemies) into one texture to improve performance.
3. **Animations:** By changing the `x1, x2, y1, y2` values over time in your `_update` loop, you can create smooth frame-by-frame animations.