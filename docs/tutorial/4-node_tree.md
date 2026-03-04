## Overview

Other then a low-level API that lets you build an entire game infrastructure yourself in lua, pika also provides a higher-level abstraction in a form of a node tree, which allows you to create and manage a tree of predetermined types of objects, attach lua scripts to them and determine the node's lifecycle. The system is similar to Godot engine as it only allows one script per node. 

Currently a nodetree can be created only from lua code, but there are plans for an xml-based scene system, which will allow you to map out a "chunk" that will be either possible to add as a child to any node or use as a part of a different xml scene.

## setup

Let's start by creating a simple node that does nothing. We can do that in the `_init()` callback of main.lua:

```lua
function _init()
    node = Node.create("node")
end
function _exit()
    node = nil
end
```

This creates a c++ object that can be directly manipulated in lua. Moreover, before it is made a child of a different node your lua code has the ownership of the c++ object. After adding the node as a child to another node, it takes ownership of the node, but the reference remains valid as long as the object still exists. Either way, it's necessary to keep track of those references and nil them once they are unnecessary. If the ownership is still in lua when program ends, an uncleaned reference could lead to memory leaks, while dead references can crash the game if you try to access them. This prevention is exactly what we do in the _exit() callback (called after the game loop was broken, right before the program ends). 

This system allows us to call this object's methods and access its properties like so:

```lua
print(node.id)
```
prints a unique id associated with this node to console


But a node that does nothing is a little boring, so let's add a script to it. Create a script with a creative name script.lua and write this:

```lua
Player = {}

function Player:_init(node)
    print("Script loaded")
    print("Caller: "..node)
end

function Player:_update(dt)
    GFX.text("one script running",100,100,4,"bob");
end

return Player
```
'Player' can be anything really, as long as it is what you return

The idea behind scripting in pika is based around lua tables. A script in pika is just a file that is executed once when a node it's attached to is created. This file **must** return a table with _init and _update functions acting as callbacks. _init is called immediately after the script is executed. It receives a node, which is an id of the node it's attached to (later in development will become a reference to said node), as well as a hidden 'self' parameter, which is, well, the tables own context. _update receives delta and also self. It's called whenever its node is updated, which is where this is going. This way a simple table in lua registry acts as a proper script.

Now we can modify our main.lua to attach a script to the node we created:

```lua
function _init()
    node = Node.create("node","script.lua")
end
function _update(dt)
    node._update(dt)
end
```

calling update on the node makes the script attached to it execute its _update callback. This is fun, but still not really a node tree. let's create an extra script called 'script2.lua'

```lua
Player2 = {}

function Player2:_init(node)
    print("Script loaded")
    print("Caller: "..node)
end

function Player2:_update(dt)
    GFX.text("other script running",0,0,4,"bob");
end

return Player2
```

And now we make changes to main.lua:

```lua
function _init()
    node = Node.create("node","script.lua")
    node2 = Node.create("node","script2.lua")
    node:add_child(node2)
end

function _update(dt)
    
    node:_update()
end

function _exit()
    node = nil
    node2 = nil
end
```

node:add_child(other: Node) makes the other node a child of the current one. The reference to the node2 remains valid, but no longer holds ownership over the object. Now its ownership belongs to its parent. Every node cleans up its children when it is destructed, but later the functionality will be extended to include methods like remove_child() for direct manipulation. Note that we only call update on 'node', not node2. That is because every node's update method calls update on every child it has. This means that in a large node tree you only need to call _update on its root and it will propagate down.