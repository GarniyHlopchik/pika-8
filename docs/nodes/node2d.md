## Overview

Node2D is a class that inherits Node. It adds 2D position coordinates and a way to get the most global position in the node's tree.
## Creation

```lua
my_node_2d = NodeFactory.create("node2d","",0,0) --nodetype, script, xpos, ypos
```

## Methods

Node2D:get_global_position() - returns a table with 2 numbers, x and y coordinates. This value is a sum of this Node's own position and its parent's global position if the parent is Node2D. For exmaple, if the root of a tree is a Node2D with position (40,70), and it has a child which is a Node2D with position (10,-20), get_global_position() will return {50,50}. The chain of addition stops as soon as the next parent either doesn't exist or isn't a Node2D.

## Note 

This class is still incomplete as it doesn't allow to freely set the position of the node outside initialization.