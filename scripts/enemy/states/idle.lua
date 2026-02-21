function extend(parent)
    local child = {}
    setmetatable(child,{__index = parent})
    return child
end

-- scripts/enemy/states/idle.lua
-- Состояние Idle: враг стоит на месте

local State = require("scripts.enemy.states.state")

-- Создаём IdleState, который наследует от State
local IdleState = extend(State)
IdleState.__index = IdleState

--- Создать новый IdleState
--- @return table новый объект IdleState
function IdleState:new()
    local self = State.new(self, "idle")
    setmetatable(self, IdleState)          -- switch metatable to IdleState
    return self
end


function IdleState:enter(enemy)
    print("[IdleState] Enemy entered idle")
end

function IdleState:update(enemy, dt)
    -- Здесь позже можно добавить:
    --   idle-анимацию (покачивание, мигание)
    --   таймер перехода в patrol
    --   проверку "вижу ли игрока" → переход в chase
end

function IdleState:exit(enemy)
    print("[IdleState] Enemy left idle")
end

return IdleState
