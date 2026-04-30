## Initial Setup
In order to start working with pika-8 you will need an executable file of the engine, which can be found in github releases or built from source code for your OS of choice using CMake. Assuming you now have it, the next step is to move the executable file to your project folder. If you use git, it would be a good call to add it to gitignore, so contributors on different OS's don't clash.  

## Configuration
Pika-8 generally doesn't require a configuration file to run. In that case it will create a window of resolution 512x512px with the window name of "Pika-8", which looks for "main.lua" file and runs it. Still, it's a good idea to get the configuration out of the way right in the beginning. Let's create a file called "config.json" in out game folder:
``` json
{
    "window_title": "Hello World!",
    "window_width": 640,
    "window_height": 480,
    "lua_script": "game.lua",
    "fonts": [
        {
            "name": "default",
            "path": "default_font.ttf"
        }
    ]
}
```

Notice that we set the lua script to "game.lua". This means that "main.lua" will no longer be used by the engine, so the file we should define is "game.lua". This is where we will write our first "hello world", but first we need to add a font to render it with. Pika uses bitmap fonts, which happen to be just a .ttf. Let's grab a font from this repo:
[Right here](../data/default_font.ttf)
Just pop it into your project root as well! The name is not to be changed.  
## Coding
We can finally get to it, create game.lua and write some code:
```lua
function _init()
    text = "Hello, Pika-8!"
    coords = { 160, 160 }
    scale = 5
    font = "default"
    color = { 255, 255, 255, 255 }
end

function _update(delta)
    GFX.cls()    

    GFX.text(text, coords, scale, font, color)
end
```

This uses an update callback, which takes in delta parameter - time elapsed since last update. Inside of it we use 2 functions from engine's GFX API: ```cls()``` and ```text(text: String, { x_position, y_position }: Table of int, scale: int, font_name: String, color: Table)```. Note that `text`, `x_position`, `y_position`, `scale` and `font_name` are required. 

Color parametr is RGBA values 0-255. You can insted replace it with `{ }` and Color will default to `{255, 255, 255, 255}`

The first one clears the screen and the second one prints a message onto the screen from our loaded font.png
