-- scripts/enemy/states/state.lua
-- State - abstract class 
-- all states (idle, move, attack) extend this class

local State = {}
State.__index = State

--- Class constructor
--- @param name string имя состояния (для отладки)
--- @return table новый объект State
function State:new(name)
    local self = setmetatable({}, State)
    self.name = name or "unnamed"
    return self
end

--- calling once when enemy enters this state
function State:enter(enemy) end

--- calling every frame
function State:update(enemy, dt) end

--- calling once when enemy enters this state
function State:exit(enemy) end

return State
