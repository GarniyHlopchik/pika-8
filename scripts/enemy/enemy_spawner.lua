--TODO optimize spawn_for_duration() (or refactoring)

local EnemySpawner = {}

-- generator
function EnemySpawner.create_generator(screen_w, screen_h, enemy_spr)
    return coroutine.create( -- ~~yield
        function()
            local i = 0 -- iterating sprites
            while true do
                local x = math.random(16, screen_w - 16)
                local y = 10
                local sprite = enemy_spr[i % #enemy_spr + 1] --BRAINFUCK lua indexes starts at 1
                coroutine.yield(x, y, sprite) -- return data
                i = i + 1
            end
        end
    )
end


function EnemySpawner.spawn_for_duration(generator, state, dt)
    -- if true, the iterator does nothing 
    if state.finished then
        return nil
    end


    state.time = state.time + dt
    state.interval_timer = state.interval_timer + dt


    if state.time > state.duration + 1 then -- +1 to spawn the last enemy from the given number of enemies
        state.finished = true
        print(string.format("[Spawner] Spawned %d enemies in %.1f seconds", state.count, state.time))
        return nil
    end


    if state.interval_timer >= state.interval then
        state.interval_timer = state.interval_timer - state.interval

        local ok, x, y, sprite = coroutine.resume(generator) -- get data from generator 
                                                             -- ok is boolean status from courutine
        if not ok then
            state.finished = true
            print("[Spawner] Generator error: " .. tostring(x)) -- x is an error message (2nd return value of resume)
            return nil
        end

        state.count = state.count + 1
        print(string.format("[Spawner] Enemy #%d at (%.0f, %.0f)", state.count, x, y))
        return x, y, sprite
    end

    return nil
end

function EnemySpawner.create_state(_duration, _interval)
    return {
        time = 0,
        interval_timer = 0, -- time from last spawn
        interval = _interval, -- interval between spawns
        duration = _duration, -- spawn during
        count = 0, -- count of spawned enemies
        finished = false -- iterator finished
    }
end

return EnemySpawner