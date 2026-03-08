## Overview

Pika's executable has several ways of loading data that attempt to apply one after another. The simplest (but not necessarily worst!) way to use pika is by placing your game files in the same directory as the executable (for config files and lua entry point, other files can be in subdirectories and used by passing paths to the engine relative to the engine's location). This is very easy to use during the game's development and allows the easiest game modification for end users, which could be both a good thing and a bad thing. In fact, this is not the only way to ship your pika game. Note that everything is based around working directory, not where the engine is physically located. So if you run it from path, it will look in your currently open directory.

When launching, the engine attempts this order of file lookup:

`Embedded -> External game.pika archive -> Bare files in the directory`

This means that when launching, the engine first tries to figure out if a game is already embedded into its binary (just appended to the end of it). If it finds a game binary attached to it, it runs off of it. Otherwise, it goes to the second option, which is looking for a game.pika file in the working directory. That file is just a renamed .zip archive with those same files and folders you create during development. If it doesn't find that too, it falls back to loading files directly from the file system, which is how we've been doing everything up to this point.

## External Zip archive packing

This way of packing your game will take you from have a bunch of files and folders you need to ship to the user down to just 2 files: the engine executable and a game.pika file. All you have to do is take all the folders and files (e.g. main.lua, sprite.png, assets folder, everything you've been working on) and pack them into a .zip archive. Now you just need to rename that file to game.pika (don't worry about changing the file extension, it does nothing to the contents of it, but that exact name is what the engine expects to see) and make sure it's in the same directory as the engine executable we want to ship it with. Now the game will run off that game.pika file. It could be a good middleground between fully embedding the game files into a single executable and having everything be openly available from scripts to images to sounds. It also enables to distribute games within pika community with a single file - if the people you share the game with all have the engine executable in their path variable, they will easily be able to check it out without having to download the engine too.

## Full embed into a single executable

Embedding can be done by hand and doesn't need any external engine tools. Embedding means creating an executable that contains your engine executable and a zip archive with all your game files. Basically, you take pika-8, take game.pika from the previous step, and slap them together into a single 'game' executable, which contains the engine's binary followed by the game's assets. Every major OS has simple commands that help you do just that.

On Linus and MacOS you use cat:

`cat pika-8 game.pika > game`

On Windows you use copy:

`copy /b pika-8.exe + game.pika game`

This produces a `game` file that can be run on its own without needing anything else.