-- scripts/enemy/enemy.lua
local IdleState = require("scripts.enemy.states.idle")

-- Class Enemy
local Enemy = {}
Enemy.__index = Enemy

--- Class constructor
--- @param x number starting position X
--- @param y number starting position Y
--- @param sprite number ID of the sprite to draw
--- @param width number width of the sprite (default 32)
--- @param height number height of the sprite (default 32)
--- @param speed number movement speed (default 50)
--- @return table new Enemy object
function Enemy:new(x, y, sprite, width, height, speed)
    local self = setmetatable({}, Enemy)

    -- pos
    self.x = x
    self.y = y

    -- sprite size
    self.width = width or 32
    self.height = height or 32

    self.speed = speed or 50
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
