function _init()
    -- Load assets, initialize game state, etc.
    spr = GFX.load("assets/sprites/player/player.png")
end

function _update(delta)
    GFX.cls()
    
    GFX.text("ABC", 60, 60, "ascii", 6, 1)
    GFX.spr(spr,60,60, 164,164, 0, 24, 0, 24)
end


-- 0123456789abcdef
-- **************** 0
-- **************** 1
-- **************** 2
-- **************** 3
-- **************** 4
-- **************** 5
-- **************** 6
-- **************** 7

--  GFX.spr(spr, pos_x, post_y, width, height, x1, x2, y1, y2)
-- x1 - start of the width cut (ex: 3)
-- x2 - end   of the width cut (ex: b)
-- y1 - start of the height cut (ex: 2)
-- y2 - end   of the height cut (ex: 5)

-- if we apply those
-- * - unused
-- # - displayed

-- 0123456789abcdef
-- **************** 0
-- **************** 1
-- ***#########**** 2
-- ***#########**** 3
-- ***#########**** 4
-- ***#########**** 5
-- **************** 6
-- **************** 7