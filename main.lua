---@diagnostic disable: lowercase-global
--#define KEY_RIGHT 262
--#define KEY_LEFT 263
--#define KEY_DOWN 264
--#define KEY_UP 265

local Enemy = require("scripts.enemy.enemy")

function _init()
    spr = GFX.load("sprite.png")
    enemy_spr = GFX.load("enemy.png")
    pos_x = 64
    pos_y = 64
    player_speed = 250.0

    -- create some enemies
    enemies = {}
    table.insert(enemies, Enemy:new(100, 200, enemy_spr))
    table.insert(enemies, Enemy:new(250, 150, enemy_spr))
    table.insert(enemies, Enemy:new(400, 300, enemy_spr))

    score = 0
end

function _update(delta)
    fps = 1/delta
    if(fps < 300) then
        print("FPS: "..fps)
    end

    GFX.cls()
    GFX.text("a quick brown fox jumps over the lazy dog", 100, 100, 3, .4)
    GFX.text("12345678", 100, 140, 3, 100)
    GFX.text(".,!?':; ()[]", 100, 180, 3, 100)
    GFX.spr(spr,pos_x,pos_y, 64,64)

    GFX.spr(spr, pos_x, pos_y, 64, 64)

    -- -- Spawn enemies every 2 seconds
    -- spawn_timer = spawn_timer + delta
    -- if spawn_timer >= 2 then
    --     table.insert(enemies, {x = math.random(10, 502), y = -16, speed = 50})
    --     spawn_timer = 0
    -- end

    -- Update enemies
    for i = #enemies, 1, -1 do
        local enemy = enemies[i]

        -- enemy.y = enemy.y + enemy.speed * delta
        -- GFX.spr(enemy_spr, enemy.x, enemy.y, 16, 16)
        -- -- Remove if off screen
        -- if enemy.y > 512 then
        --     table.remove(enemies, i)
        -- end
        -- -- Check collision
        -- if pos_x < enemy.x + 16 and pos_x + 64 > enemy.x and pos_y < enemy.y + 16 and pos_y + 64 > enemy.y then
        --     print("Hit!")
        --     score = score + 1

        -- enemy:update(delta)
        -- enemy:draw()

        if not enemy.alive then
            table.remove(enemies, i)
        end
    end

    --TODO debug
    if #enemies > 0 then
        GFX.text("enemy state: " .. enemies[1]:get_state_name(), 10, 10, 2, 0.4)
    end


    -- player movement
    if Input.btnp(262) then
        pos_x=pos_x+(player_speed*delta)
    end
    if Input.btnp(263) then
        pos_x=pos_x-(player_speed*delta)
    end
    if Input.btnp(264) then
        pos_y=pos_y+(player_speed*delta)
    end
    if Input.btnp(265) then
        pos_y=pos_y-(player_speed*delta)
    end
end