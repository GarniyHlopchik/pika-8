--#define KEY_RIGHT 262
--#define KEY_LEFT 263
--#define KEY_DOWN 264
--#define KEY_UP 265

function _init()
    spr = GFX.load("sprite.png")
    pos_x = 64
    pos_y = 64
end
function _update(delta)
    fps = 1/delta
    if(fps < 300) then
        print("FPS: "..fps)
    end
    GFX.cls()
    GFX.spr(spr,pos_x,pos_y)

    if Input.btnp(262) then
        pos_x=pos_x-(100.0*delta)
    end
    if Input.btnp(263) then
        pos_x=pos_x+(100.0*delta)
    end
    if Input.btnp(264) then
        pos_y=pos_y-(100.0*delta)
    end
    if Input.btnp(265) then
        pos_y=pos_y+(100.0*delta)
    end
end