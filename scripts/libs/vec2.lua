local Vec2 = {}
Vec2.__index = Vec2

--- Constructor for Vec2
--- @param x number
--- @param y number
--- @return Vec2
function Vec2:new(x,y)
    if type(x) ~= "number" or type(y) ~= "number" then
        error("Vec2:new expects numeric x and y, got x: " .. type(x) .. ", y: " .. type(y))
    end

    local self = setmetatable({}, Vec2)
    self.x = x or 0
    self.y = y or 0
    return self
end

----------------------------------------------------------------------------------------------------

--#region Vector Arithmetic

function Vec2:__add(other)
    if getmetatable(other) ~= Vec2 then
        error("Attempt to add Vec2 with non-Vec2 value")
    end

    return Vec2:new(self.x + other.x, self.y + other.y)
end

function Vec2:__sub(other)
    if getmetatable(other) ~= Vec2 then
        error("Attempt to subtract Vec2 with non-Vec2 value")
    end

    return Vec2:new(self.x - other.x, self.y - other.y)
end

---Multiplication by scalar
function Vec2:__mul(other)
    if type(other) == "number" then
        return Vec2:new(self.x * other, self.y * other)
    end
    error("Attempt to multiply Vec2 with non-number value")
end

function Vec2:__div(other)
    if type(other) == "number" then
        if other == 0 then
            error("Division by zero in Vec2 division")
        end
        return Vec2:new(self.x / other, self.y / other)
    end
    error("Attempt to divide Vec2 with non-number value")
end

function Vec2:__unm()
    return Vec2:new(-self.x, -self.y)
end

function Vec2:__eq(other)
    if getmetatable(other) ~= Vec2 then
        return false
    end
    return self.x == other.x and self.y == other.y
end

--#endregion

----------------------------------------------------------------------------------------------------

function Vec2:__tostring()
    return string.format("Vec2(%.2f, %.2f)", self.x, self.y)
end

----------------------------------------------------------------------------------------------------

--#region Geometry

---Vector Length
---@return number
function Vec2:length()
    return math.sqrt(self.x * self.x + self.y * self.y)
end

---Vector Length Squared
---@return number
function Vec2:length_sq()
    return self.x * self.x + self.y * self.y
end

---Normalize Vector
---@return Vec2
function Vec2:normalize()
    local len = self:length()
    if len == 0 then
        return Vec2:new(0, 0)
    end
    return self / len
end

---Dot Product
---@param other Vec2
---@return number
function Vec2:dot(other)
    if getmetatable(other) ~= Vec2 then
        error("Attempt to dot Vec2 with non-Vec2 value")
    end
    return self.x * other.x + self.y * other.y
end

--- Cross Product (returns scalar "z" component in 2D)
--- If >0 then self is counter-clockwise from other, if <0 then clockwise, if 0 then collinear
---@param other Vec2
---@return number
function Vec2:cross(other)
    if getmetatable(other) ~= Vec2 then
        error("Attempt to cross Vec2 with non-Vec2 value")
    end
    return self.x * other.y - self.y * other.x
end

--- Distance to another vector
---@param other Vec2
---@return number
function Vec2:distance(other)
    if getmetatable(other) ~= Vec2 then
        error("Attempt to distance Vec2 with non-Vec2 value")
    end
    return (self - other):length()
end

--- Angle of the vector in radians (0 along positive x-axis, counter-clockwise)
---@return number
function Vec2:angle()
    return math.atan(self.y, self.x)
end

--- Rotate the vector by a given angle in radians (positive is counter-clockwise)
---@param rad number
---@return Vec2
function Vec2:rotate(rad)
    local x = self.x * math.cos(rad) - self.y * math.sin(rad)
    local y = self.x * math.sin(rad) + self.y * math.cos(rad)
    return Vec2:new(x, y)
end

--#endregion

----------------------------------------------------------------------------------------------------

--#region Utils

--- Linear interpolation between this vector and another by t (0 to 1)
---@param other Vec2
---@param t number
---@return Vec2
function Vec2:lerp(other, t)
    if getmetatable(other) ~= Vec2 then
        error("Attempt to lerp Vec2 with non-Vec2 value")
    end
    return (1 - t) * self + other * t
end

--- Clamp the length of the vector between min and max
--- @param min number
--- @param max number
--- @return Vec2
function Vec2:clamp(min, max)
    local len = self:length()
    if len == 0 then
        return Vec2:new(0, 0)
    end
    local clamped_len = math.max(min, math.min(max, len))
    return self * (clamped_len / len)
end

--- Floor the components of the vector (useful for pixel coordinates)
---@return Vec2
function Vec2:floor()
    return Vec2:new(math.floor(self.x), math.floor(self.y))
end

--- Create a copy of the vector (Lua tables are mutable)
---@return Vec2
function Vec2:copy()
    return Vec2:new(self.x, self.y)
end

--#endregion

----------------------------------------------------------------------------------------------------

--#region Static Constructors

---Unit vector from angle in radians
---@param rad number
---@return Vec2
function Vec2.from_angle(rad)
    return Vec2:new(math.cos(rad), math.sin(rad)) -- cos^2 + sin^2 = 1, so this is a unit vector
end

Vec2.ZERO = Vec2:new(0, 0)
Vec2.ONE = Vec2:new(1, 1)

-- Screen coordinates: y increases downwards, so UP is (0, -1) and DOWN is (0, 1)
Vec2.UP = Vec2:new(0, -1)
Vec2.DOWN = Vec2:new(0, 1)
Vec2.LEFT = Vec2:new(-1, 0)
Vec2.RIGHT = Vec2:new(1, 0)

--#endregion

return Vec2