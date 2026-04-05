## Preparing the Asset
Now that we have created a window and successfully displayed text, it's time to get something more visual on the screen - like a sprite representing your player character. 

First, you'll need an image file. You can draw a simple character or grab a placeholder and place it in your project folder right next to your `game.lua` file. 

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
function _init()
    player = GFX.load("player_sprite.png")
    sprite = GFX.spr(my_texture, 50, 50, 48, 48, {3, 11, 2, 5}, {255, 255, 255, 255})
end

function _update(delta)
    GFX.cls()

    sprite:draw()
end

```

Just like before, `GFX.cls()` wipes the screen clean.
Then, we call a new function: `GFX.spr(texture, x, y, [width], [height], {x1, x2, y1, y2}, {r, g, b, a})`. It returns a reference for our sprite that we can later use for display:

| *Function*            | *Parametr*                            | *Description*                         |
| :-------------------- | :-----------------------------------: | :------------------------------------ |
| `sprite:draw`         | ---                                   | draws the sprite                      |
| `sprite:pos`          | `(new_x: int, new_y: int)`            | changes the position                  |
| `sprite:size`         | `(new_width: int, new_height: int)`   | changes the size                      |
| `sprite:scaleXY`      | `(w: float, l: float)`                | changes the size based on the size    |
| `sprite:scale`        | `(s: float)`                          | changes the size based on the size    |
| `sprite:color`        | `( {r, g, b, a}: {int [0-255]} )`     | changes the color                     |
| `sprite:uv`           | `( { x1, y1, x2, y2}: {float} )`      | changes the cutting                   |
| `sprite:texture`      | `( texture_path: String )`            | changes the displayed texture         |
| `sprite:visible`      | `(bool)`                              | should it be displayed                |
| `sprite:mirror`       | `(bool, bool)`                        | flips horizontaly and verticaly       |
| `strite:rotation`     | `(degree: int)`                       | sets a rotation                       |
| `sprite:pivot`        | `(x: int, y: int)`                    | sets a pivot(origin).                 |



GFX.spr function is quite flexible itself. Let's break down its arguments:

```lua
GFX.spr(texture, x, y, [width], [height], {x1, x2, y1, y2}, {r, g, b, a})
```

| Parametr          | Accepted type(s) | Required |
| :---              | :---:         | :---      |
| `texture`         | `int`         | `- [x]`   | 
| `x`               | `float`       | `- [x]`   |
| `y`               | `float`       | `- [ ]`   |
| `width`           | `float`       | `- [ ]`   |
| `height`          | `float`       | `- [ ]`   |
| `{x1, y1, x2, y2}`| `{ float }`   | `- [ ]`   |
| `{r, g, b, a}`    | `{ float }`   | `- [ ]`   |

> [!IMPORTANT]
> Even though many parametrs exept `float` values, it's *highly* recomended to use integer insted.

Run the engine, and you should now see your player sprite sitting perfectly on the screen!