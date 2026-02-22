---@diagnostic disable: lowercase-global, undefined-global
--#define KEY_RIGHT 262
--#define KEY_LEFT 263
--#define KEY_DOWN 264
--#define KEY_UP 265

-- Подключаем класс Enemy
local Enemy = require("scripts.enemy.enemy")

function _init()
    --sound playing example
    sound = SFX.load("pipe.mp3");
    SFX.play(sound);
    
    spr = GFX.load("sprite.png")
    enemy_spr = GFX.load("enemy.png")
    pos_x = 64
    pos_y = 64
    player_speed = 250.0
    enemies = {}
    spawn_timer = 0
    score = 0
end

function _update(delta)
    fps = 1/delta
    if(fps < 300) then
        -- print("FPS: "..fps)
    end
    GFX.cls()

    -- GFX.text("a quick brown fox jumps over the lazy dog", 100, 100, 3, .4)
    -- GFX.text("12345678", 100, 140, 3, 100)
    -- GFX.text(".,!?':; ()[]", 100, 180, 3, 100)

    GFX.text("score: " .. score, 10, 30, "default", 2)
    GFX.spr(spr,pos_x,pos_y, 64,64)

    -- Spawn enemies every 2 seconds
    spawn_timer = spawn_timer + delta
    if spawn_timer >= 2 then
        local e = Enemy:new(math.random(10, 502), 10, enemy_spr)
        table.insert(enemies, e)
        spawn_timer = 0
    end

    -- Update enemies
    for i = #enemies, 1, -1 do
        local enemy = enemies[i]
        enemy:update(delta)
        enemy:draw()
        -- Remove if off screen
        if enemy.y > 512 then
            enemy.alive = false
        end

        -- Проверка коллизии с игроком
        if pos_x < enemy.x + enemy.width and pos_x + 64 > enemy.x
            and pos_y < enemy.y + enemy.height and pos_y + 64 > enemy.y then
            print("Hit!")
            score = score + 1
            enemy.alive = false
        end

        -- Убираем мёртвых
        if not enemy.alive then
            table.remove(enemies, i)
        end
    end

    if #enemies > 0 then
        GFX.text("enemies: " .. #enemies .. "  state: " .. enemies[1]:get_state_name(), 10, 10, "default", 2, 0.4)
    else 
        GFX.text("enemies: 0", 10, 10, "default", 2, 0.4)
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