# Lua as the language choice

## Context

Different engines employ different approaches to scripting the game logic. Some engines contain a compilation toolchain that lets the developer compile their game logic code directly into machine code. That includes UnrealEngine, CryEngine (both use C++). On the other hand, some engines embed an interpreter of source/bytecode and that source code. While that makes the performance worse by some margin, it is way more convenient to develop a game in such a way. Examples of this are Godot (GDScript/C#), Unity (C#), love 2D (lua).

## Decision

For the engine, we went with interpreting code by embedding a Lua interpreter into the engine. While it has implications for performance, it makes the development easier than compiling the scripts. Lua specifically was chosen because of the sheer amount of engines that already use it and have a community of developers capable of writing lua code. That includes Defold, Love2D, PICO-8, TIC-80, as well as the engine used by Roblox. 

## Status

Active.

## Consequences

The Lua interpreter turned out to have great compatibility with the rest of the c++ codebase due to a serious api for calling both lua from c++ and c++ from lua. Sol2 also turned out to be a very strong library for manipulating c++ objects in lua.

# Entry points and tables-as-scripts

Most engines allow to attach scripts to objects for more modular logic approach. 

## Decision

In pika-8, there is a simple contract that makes .lua file eligible to be an attached script. That requirement is that the code in the file needs to return a *table*. In lua, tables have similar functionality to objects in javascript. That means that a key-value pair of a table can be any value type, including a function. Functions can be used as callbacks in a script, such as _init and _update. They receive *self* - a hidden reference to the table it exists in. The dotted syntax (e.g. self.x = 10) looks very elegant, but all it does is add a key-value pair to the table. 

## Consequences

This turned out to be a good decision, both easy to implement and having all the functionality it needs to have.