function _update(delta)
    GFX.cls()    
    --  
    GFX.text(" !\"#$%&'()*+,-./", 60, 60, "default", 6, 1)
    GFX.text("0123456789:;<=>?", 60, 120, "default", 6, 1)
    GFX.text("@ABCDEFGHIJKLMNO", 60, 180, "default", 6, 1)
    GFX.text("PQRSTUVWXYZ[\\]^_", 60, 240, "default", 6, 1)
    GFX.text("`abcdefghijklmno", 60, 300, "default", 6, 1)
    GFX.text("pqrstuvwxyz{|}~", 60, 360, "default", 6, 1)
end