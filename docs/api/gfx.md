### GFX

A low-level graphics API for Pika. It allows to render items onto the screen.

## GFX.cls()

Takes no arguments. Clears the screen. The correct usage is putting it before any other rendering functions to make sure that nothing is drawn over the previous frame.

## GFX.load(path: String) -> int

Loads a sprite onto the GPU from the specified path. Most image formats, such as .png, .jpeg, .gif are supported. Returns an integer that should be used to render the sprite onto the screen.

## GFX.spr(texture: int, x: Number, y: Number, width: Number, height: Number, u1:float,v1:float,u2:float,v2:float)

Renders a sprite onto the screen. `Texture` is an integer received from the `load()` function, which tells the engine what sprite needs to be called. X and Y are screen coordinates to render to. (0,0) point is on top-left corner of the screen. The rest of the arguments are optional: width and height of the sprite, defaults to 8x8; u1,v1,u2,v2 correspond to the uv coordinates you want to render the sprite from. By default it is set to render the entire sprite, but manipulating these values can help to work with spritesheets.

## GFX.text(text: String, x: Number, y:Number, font_name: String, scale: Number,spacing: Number)

Renders `text` onto the screen. X and Y are screen rendering coordinates, font_name corresponds to the font_name set up in config.json, scale is the size of the characters, and spacing is the space between words. 