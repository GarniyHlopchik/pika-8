## Draw Debug workflow

You can use this library to get quick debugging done as to where something is on the screen, whether something overlaps etc. Debug shapes are drawn before the main draw loop. Do not use this for production.

```lua
function _update(delta)
   --params: start_position, end_position, color
   Debug.draw_line( {30, 30}, {120, 60}, {255, 255, 255, 128} )

   --params: center_position, radius, color
   Debug.draw_circle( {250, 400}, 30, {128, 178, 255, 255} )

   --params: start_position, end_position, color
   Debug.draw_rect( {50, 50}, {250, 150}, {255, 255, 0.67, 128})

   -- params: vertice amount, side length, center position, color
   Debug.draw_polygon( 6, 50, {150, 150}, {128, 178, 255, 255} )

   --params: vertice amount, [positions], color
   Debug.draw_free_shape( 3, {10,20}, {40, 50}, {0, 100}, {128, 178, 255, 255} )
end
```