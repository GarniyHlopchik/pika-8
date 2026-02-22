function _init()
    sound = SFX.load("monster.mp3");
    sound1 = SFX.load("Its_Going_Down_Now.mp3");
    sound2 = SFX.load("Kingslayer.mp3");

    time = 0;

end

function _update(delta)
    GFX.cls()    
    --  
    -- GFX.text("Abc", 60, 60, "PixBobLite", 6, 1)

    time = time + delta;

    -- print("time: " .. math.floor(time));

    if math.floor(time) == 3 then
        SFX.play(sound1, 1.0, 1.0, false, 0.0);
        GFX.text("play 1", 60, 60, "PixBobLite", 3, 1)
    end

    if math.floor(time) == 5 then
        SFX.play(sound2, 1.0, 1.0, false, 0.0);
        GFX.text("play 2", 60, 120, "PixBobLite", 3, 1)
    end

    if math.floor(time) == 8 then
        SFX.play(sound3, 1.0, 1.0, false, 0.0);
        GFX.text("play 3", 60, 180, "PixBobLite", 3, 1)
    end

    
    if math.floor(time) == 12 then
        SFX.stop(sound);
        GFX.text("stop 1", 60, 60, "PixBobLite", 3, 1)
    end
    if math.floor(time) == 15 then
        SFX.stop(sound1);
        GFX.text("stop 2", 60, 120, "PixBobLite", 3, 1)
    end
    if math.floor(time) == 18 then
        SFX.stop(sound2);
        GFX.text("stop 3", 60, 180, "PixBobLite", 3, 1)
    end

end