## Overview

Node is a base class that every other type of node extends from. It handles basic Nodetree hierarchy, handles execution of the script attached to it and propagates its own _update call. Update propagation is what sets pika's nodetree system apart from the others, as `Tree` is not a separate concept. Instead, the upper-most node of a tree is considered its root, allowing for multiple trees to be created and updated as needed.

## Creation
```lua
my_node = NodeFactory.create("node","script_name.lua")
```

This creates a node with script `"script_name.lua"` attached to it. If you don't want to attach a script to a created node, you should put empty quotes in that parameter:

```lua
my_node = NodeFactory.create("node","")
```

## Attach-Scripting overview

An attachable lua script is a .lua file that returns a table. Such table provides functions and enclosed context necessary for videogame scripting. For example:

```lua
--script_name.lua

Script = {}
function Script:_init(node) --script's init is called when the node is created
    self.owner = node --save the ref to owner to be able to access later
end

function Script:_update(dt) --script's update is called whenever its owner's update is called
    self.parent = self.owner.parent
    if self.parent then 
        print("I am not batman!")
    end
end

return Script
```

As you can see, while this technically returns a simple lua table, it looks and functions like a real script. Callbacks receive the self parameter, which is a reference to that table itself, adding key-value pairs perfectly handles creating variables that persist between function calls and have proper comfortable syntax. That returned table is then stored in the lua registry, and a handle to it is given to the node that owns it. Not only does it allow the node to call this script's callbacks, but it also lets other scripts access this script as will be described below.

## Node Properties
Node.parent - reference to node's parent. If there is no parent.. Idk, probably nil
Node.children - reference to node's children. Can be iterated through like a normal table-array in lua
Node.script - reference to the node's script. As mentioned above, it's just a table. Considering this, every member and function is "public". This is good for accessing/changing things that are meant to be changed (like calling take_damage() on an enemy script), but make sure that your redefinitions don't break the other script's integrity to avoid kaboom.

## Node methods

Node:add_child(other) - adds another node as child of Node.
Node:remove_child(other) - removes the child - not just the link between the nodes, but it actually removes the child node from memory. Using your `other` reference after that will lead to kaboom
Node:_update(delta) - calls _update on this node and its children. In turn, the children will call it on their children and thus every node that is child of this one will be updated. Calling _update also calls the _update callback on the script attached to the node if there is such. _update is meant to be called on nodes that are roots of some sort of node tree you need to update. You don't need to call _update on every node you create, or else, well, probably kaboom.
