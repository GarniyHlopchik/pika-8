### Pika-8
## What are we?

Pika-8 is a 2D game engine written in c++ using OpenGL, which is programmed using Lua programming language. Pika-8 provides API's for both low-level and high-level videogame programming. Pika-8 executable file is designed both for direct execution, in which case it looks for files associated with your game in its current directory, as well as usage from the path variable with the abilty to specify a directory to execute on. In the end it functions as an editor-free programming environment with tools for game creation.  

## Editor-free design

Pika-8 does **not** have an editor like most other game engines. Instead it is explicitly designed to not *need* it. Scene tree creation, scripting, map creation, spritesheet definition can only be done using human-readable text formats. This has advantages of requiring less of a learning curve to get into, being more git-friendly (say no to the chore of clearing up godot scene merge conflicts), and allowing you to create your games in any IDE of your choice.  

## Should you even encode your code?

Building a game in most engines requires encoding the game's code and assets into unreadable data or binary libraries, or even embedding it into the final executable itself. It is necessary for some videogames that care deeply about file integrity, such as online games. But if the entire source code of the game remains fully accessable and untouched in your final games, it opens up wide modding possibilities of all sorts. Modders will no longer need to create complicated tools and loaders to modify your game, but instead they will have the same tools *you* had when making it in the first place. Pika-8 allows to do just that: the engine's executable can be shipped along with the source code, turning it into your actual game build.  

## Who even needs it?

There is a large community of game developers associated with "fantasy console" game engines, such as PICO-8 and TIC-80, both of which provide a low-level API that is programmed in lua programming language. These engines intentionally impose limitations, such as fixed color palette, limited code amount, limited spritesheet size and audio channels. Developers coming from such environments could need a next step that combines familiar patterns with new possibilities. Pika is designed to be similar in coding to those low-level engines, but with no strict limitations, which also eases an unexperienced developer into more high-level engine practices. It still doesn't lock you into those high-level abstractions, so they can be avoided entirely.  

For further info, look into docs.