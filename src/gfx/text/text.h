#pragma once
#include "gfx/gfx.h"
#include "gfx/gfx_struct.h"
class Sprite;
#include <string>
#include <unordered_map>

class Text {
public:
  explicit Text(GFX& gfx);
  ~Text();

  void draw_text(const std::string& text, float x, float y, const std::string& font_name, float scale, Color color);

  void load_font(const std::string& font_name);
  bool is_font_loaded(const std::string& font_name);

private:
  GFX& gfx;
  std::unordered_map<std::string, LoadedFont> font_cache;
};