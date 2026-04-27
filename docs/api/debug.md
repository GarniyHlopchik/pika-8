## Draw Debug workflow

You can use this library to get quick debugging done as to where something is on the screen, whether something overlaps etc. Debug shapes are drawn before the main draw loop. Do not use this for production.

```lua
function _update(delta)
   --params: start_position, end_position, color
   Debug.draw_line({30,30},{120, 60},{1,1,1,0.5})
   --params: center_position, radius, color
   Debug.draw_circle({250, 400}, 30, {0.5, 0.7, 1.0, 1.0})
   --params: start_position, end_position, color
   Debug.draw_rect({50,50},{250, 150},{1,1,0.67,0.5})
   --params: vertice amount, [positions], color
   Debug.draw_polygon(3, {10,20}, {40, 50}, {0, 100}, {0.5, 0.7, 1.0, 1.0})
end
```