## Overview

YSort is a class that extends Node2D. It doesn't add any new functionality, but rewrites the way _update call propagates to child nodes. This node is meant to be used the same way as the YSort node in Godot 3.x.x. : any Node2D's that are direct children of this node will get y-sorted. It comes down to the order of propagation in _update: YSort node basically updates its direct children in the order of y position increase: first those with lower y value, then with higher (y axis is pointing down in pika).

## Creation

```lua
sorter = NodeFactory.create("ysort","",0,0)
```