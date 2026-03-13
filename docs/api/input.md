### Input

Input collection API in Pika.

## btn(id:int) -> bool

Function that takes in key id and return true if that key is currently pressed, or false if it isn't. Keys are defined in a table at the end of the page.

## btnp(id:int) -> bool

Function that takes in the key id and returns true if the key was just pressed, and false if it isn't so

## mpos() -> x,y

A function that returns current screen position of the mouse.

Go to https://wiki.libsdl.org/SDL3/SDL_Scancode for a table that shows which integers map to which keys.