## Suggested nodetree workflow

As discussed in the previous page, the engine has basic functionality for creating and managing nodetrees. Still, this abstraction isn't as robust as most other game engines, so you should consider some kind of workflow when dealing with it. While this is purely a suggestion, these ideas might help to design your games better.

## Handle your root(s)

For starters, in pika there is no concept of a SceneTree or root by itself. Any node's update can be called from lua reference using `node._update(dt)` and will propagate down to any children it has. This means that you are the one creating a root node, making sure it's deleted properly and deciding when it should update. 

The easiest thing you can do is simply create one root node, update it in main.lua's _update(dt) callback and delete it before the program closes in a `_exit()` callback. It would look something like that:

``` lua
function _init()
    root = Node.create("node")
end

function _update(dt)
    root._update(dt)
end

function _exit()
    root = nil --deletes the reference, but if a node has no parent, lua *owns* it. So deleting the reference clears out the node
end
```

You can do a lot of experimenting with this to create a system that fits you. For example, different parts of your game that you want to always be loaded, but not functioning at the same time, can be configured as separate roots:

```lua
function _init()
    game_states = {
        EXPLORE = 1,
        BATTLE = 2,
        PAUSE_MENU = 3
    }
    state = game_states.EXPLORE

    explore = Node.create("node")
    battle = Node.create("node")
    pause = Node.create("node")
end

function _update(dt)
    if state == game_states.EXPLORE then
        explore._update(dt)
    else if state == game_states.BATTLE then
        battle._update(dt)
    else 
        pause._update(dt)
    end
end

function _exit()
    explore = nil
    battle = nil
    pause = nil
end
```

## Creation functions

The easiest way to create something that acts like a separate scene is by writing a function, calling which appends a certain node structure to a specified node. For example:

```lua

function create_enemy(node)
    enemy = Node.create("node","scripts/enemy.lua")
    create_sprite(enemy) --assuming this is another creation function that already exists
    create_collision(enemy) --and this
    create_hitbox(enemy) --and this
    node.add_child(enemy)
    return enemy --return can be necessary for referencing created structures to handle future deletion
end
```

You can do a lot of adjastments to create any structure you need for your game. For example, if a script we're attaching to a node has a function we can call, we can immediately pass necessary references:

```lua
--scripts/enemy.lua
Enemy = {}
function Enemy:_init(owner)
    self.owner = owner
end
function Enemy:_ref(sprite)
    self.sprite = sprite
end

return Enemy

--create_enemy.lua
function create_enemy(node)
    enemy = Node.create("node","scripts/enemy.lua")
    spr = create_sprite(enemy)
    enemy.script._ref(spr) --pass sprite to enemy
    create_collision(enemy) 
    create_hitbox(enemy)
    node.add_child(enemy)
    return enemy
end
```

If we wish to, we can even wrap it into a factory-like function that decides what to create using some other data or random:

```lua
function create_random_enemy(node)
    rand = math.random(0,9)
    result = nil
    if rand < 3 then
        result = create_strong_enemy(node)
    else 
        result = create_enemy(node)
    end
    return result
end
```