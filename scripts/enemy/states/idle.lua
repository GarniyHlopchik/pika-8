function extend(parent) -- extend (OOP)
    local child = {}
    setmetatable(child,{__index = parent})
    return child
end


local State = require("scripts.enemy.states.state")

-- IddleState extends State
local IdleState = extend(State)
IdleState.__index = IdleState

-- constructor
function IdleState:new()
    local self = State.new(self, "idle")
    setmetatable(self, IdleState)          -- switch metatable to IdleState
    return self
end


function IdleState:enter(enemy)
    print("[IdleState] Enemy entered idle")
    enemy.idle_timer = 0
    enemy.base_y = enemy.y -- base_y and timer are bound to an object
end

function IdleState:update(enemy, dt)
    enemy.idle_timer = enemy.idle_timer + dt
    enemy.y = enemy.base_y + math.sin(enemy.idle_timer * 2) * 3
    -- Здесь позже можно добавить:
    --   таймер перехода в patrol
    --   проверку "вижу ли игрока" → переход в chase
end

function IdleState:exit(enemy)
    enemy.y = enemy.base_y -- reset y position
    print("[IdleState] Enemy left idle")
end

return IdleState
