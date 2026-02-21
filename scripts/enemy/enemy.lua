-- scripts/enemy/enemy.lua
local IdleState = require("scripts.enemy.states.idle")

-- Class Enemy
local Enemy = {}
Enemy.__index = Enemy

--- Class constructor
--- @param x number начальная позиция X
--- @param y number начальная позиция Y
--- @param speed number скорость движения
--- @param sprite number ID текстуры (от GFX.load)
--- @return table новый объект Enemy
function Enemy:new(x, y, sprite)
    local self = setmetatable({}, Enemy)

    -- pos
    self.x = x
    self.y = y

    -- sprite size
    self.width = 16
    self.height = 16

    self.speed = 50
    self.sprite = sprite
    self.alive = true

    -- State machine
    self.state = nil          -- current state (State object)
    self:set_state(IdleState:new())

    return self
end

--- switch state
function Enemy:set_state(new_state)
    -- exit old state
    if self.state then
        self.state:exit(self)
    end

    -- switch to new state
    self.state = new_state

    -- enter to new state
    self.state:enter(self)
end

--- dt (delta time), every frame
function Enemy:update(dt)
    if not self.alive then return end

    -- logic to current state
    if self.state then
        self.state:update(self, dt)
    end
end

--- draw enemy
function Enemy:draw()
    if not self.alive then return end
    GFX.spr(self.sprite, self.x, self.y, self.width, self.height)
end

--- for debug
function Enemy:get_state_name()
    if self.state then
        return self.state.name
    end
    return "none"
end

return Enemy
