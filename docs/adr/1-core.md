# C++ as the main programming language

## Context

A game engine poses several challenges that direct a developer towards one toolset rather than the other. First of all, it needs to run as natively as possible on any hardware and operating system without requiring any external runtimes or tools. This rules out interpreted and JIT-compiled languages, such as Python, JavaScript, Java and C#. Secondly, it needs to be performant for real-time rendering and logic processing. This rules out compiled languages with serious runtime, like GoLang. For most game engines, extensive possibilites of asynchronous resource loading are expected in order not to block the main thread during loading and provide better performance. Lastly, we need to account for a toolset that allows to build GUI-based applications that are as stable as possible and communicate directly with the graphics hardware and the operating system(s) - ideally, in a unified way that allows cross-platform development for desktop OS's (Windows, Linux, MacOS), mobile ones (Android, IOS) and Web as an accessibility option (Web building is a big part of indie game development space on platforms like [itch.io](https://itch.io/) and [GameJolt](https://gamejolt.com/) as it doesn't require downloading any exetuable files that might be unsafe and malicious, plus running directly in the browser proves more convenient for smaller projects). 

## Decision

After crossing out everything that doesn't fit, we are left with either C or C++. Some other compiled languages like Zig, Rust and Vlang also don't exactly fit the requirements when it comes to having battle-tested libraries that deal with rendering (although bindings are probably available). After not-so-long consideration, C++ was chosen for its object-oriented design, broad standard library, broad choice of C-based libraries and fast execution speeds.

## Status

Active.

## Consequences

Using C++ lead the project to a not-so-pretty codebase, but one that is performant and stable.

# Framework-like design

## Context

Game engines take on different shapes. Most of them have a dedicated editor for managing levels(/scenes), sometimes scripting (Godot, Unity, Unreal Engine). While this is often efficient for big projects, it also locks the developer behind a single editor and workflow. For smaller projects or prototypes this might just be less convenient.

## Decision 

This engine takes a different approach, similar to [Love2D](https://love2d.org/). There is no editor, and all the game development can be done within the IDE of your choice. The workflow intends that there is either an executable file of the engine in the project folder or it exists in a single instance and is added to PATH variable. When the developer needs to test their creation, they type .\pika-8 (or ./pika-8 on Linux, or just pika-8 if added to path). After that the game runs off of config and script files located in the project folder, specifically by running a single "entry-point" file which can be configured within the config.json file. This way much more architectural decisions can be layed onto the developer.

## Status

Active.

## Consequences

Such design decision ended up having pros and cons. It is in fact easier to get a game going than in most engines, but the editor-less design lacks in where, well, an editor is needed. For example, it's difficult to edit maps. While it is possible to get away in 2D games, which for now are the focus of the engine (for that text files can be used where a tile is represented by a character. For example, we decide on the fact that "." corresponds to walkable tiles and "=" corresponds to a wall), in 3D it would prove totally impossible or unreasonably difficult. 

# Main loop implementation

Currently the main loop is implemented manually with different phases being invoked in an order. 
