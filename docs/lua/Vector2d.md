# Vec2 class strurture:

```
Vec2
├── Constructor
│   └── Vec2:new(x, y) → Vec2           -- or Vec2(x, y) via __call [WIP]
│
├── Arithmetic metamethods
│   ├── __add(a, b) → Vec2              -- vec + vec
│   ├── __sub(a, b) → Vec2              -- vec - vec
│   ├── __mul(a, b) → Vec2              -- vec * number  or  number * vec
│   ├── __div(a, b) → Vec2              -- vec / number
│   ├── __unm(a)    → Vec2              -- -vec (negation)
│   └── __eq(a, b)  → bool             -- vec == vec
│
├── Utility metamethods
│   └── __tostring() → "Vec2(x, y)"    -- for print() / debugging
│
├── Geometry
│   ├── :length()         → number      -- length (modulus)
│   ├── :length_sq()      → number      -- length² (without sqrt, for comparisons)
│   ├── :normalize()      → Vec2        -- unit vector (new)
│   ├── :dot(other)       → number      -- dot product
│   ├── :cross(other)     → number      -- "z" pseudo-cross in 2D
│   ├── :distance(other)  → number      -- distance to another vector
│   ├── :angle()          → number      -- vector angle (atan2)
│   └── :rotate(rad)      → Vec2        -- rotation by radians
│
├── Utils
│   ├── :lerp(other, t)   → Vec2        -- linear interpolation
│   ├── :clamp(min, max)  → Vec2        -- length limitation
│   ├── :floor()          → Vec2        -- round down (for pixels)
│   └── :copy()           → Vec2        -- copy (Lua tables are mutable!)
│
└── Static constructors
    ├── Vec2.from_angle(rad)  → Vec2    -- unit vector from angle
    ├── Vec2.ZERO             → Vec2(0, 0)────┐
    ├── Vec2.ONE              → Vec2(1, 1)    │
    ├── Vec2.UP               → Vec2(0, -1)   ├── screen coordinates
    ├── Vec2.DOWN             → Vec2(0, 1)    │
    ├── Vec2.LEFT             → Vec2(-1, 0)   │
    └── Vec2.RIGHT            → Vec2(1, 0)────┘
```