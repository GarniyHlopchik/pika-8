## Preparing the Asset
Now that we have created a window and successfully displayed text, it's time to get something more visual on the screen—like a sprite representing your player character. 

First, you'll need an image file. You can draw a simple character or grab a placeholder, just make sure to name it `player_sprite.png` and place it in your project folder right next to your `game.lua` file. 

## Initialization
Before we can draw the sprite, the engine needs to load the texture into memory. Loading files every single frame is bad for performance, so we want to do this only once when the game boots up. 

Pika-8 provides an `_init()` callback exactly for this purpose. Let's open `game.lua` and set up our player variable:

```lua
function _init()
    player = GFX.load("player_sprite.png")
end
```

Here we are utilizing the `GFX.load(path: String)` function from the engine's API. The `path` parameter simply points to the location and name of the image you want to load, storing the resulting texture data inside our `player` variable.

## Rendering the Sprite

With the texture loaded into memory, we can finally display it. To do this, we will use the `_update(delta)` callback from the previous tutorial.

Every frame, we need to clear the old screen and draw our sprite at its designated coordinates:

```lua
function _update(delta)
    GFX.cls()
    GFX.spr(player, 50, 50, 48, 48)
end

```

Just like before, `GFX.cls()` wipes the screen clean.
Then, we call a new function: `GFX.spr(texture, x, y, [width], [height], [u1], [v1], [u2], [v2])`.

This function is quite flexible. Let's break down its arguments:

* **texture (required):** The texture variable we loaded in `_init()`.
* **x & y (required):** The screen coordinates where the sprite will be drawn.
* **width & height (optional):** The dimensions to render the sprite. If left blank, Pika-8 defaults to an 8x8 size.
* **u1, v1, u2, v2 (optional):** The UV coordinates (ranging from 0 to 1). These are used if your image is a sprite sheet and you only want to draw a specific section of it. By default, it draws the entire image (0, 0, 1, 1).

Run the engine, and you should now see your player sprite sitting perfectly on the screen!