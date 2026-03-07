#pragma once

#include "gfx/gfx.h"
#include "gfx/gfx_struct.h"

#include <string>
#include <unordered_map>

class Text {
public:
  explicit Text(GFX& gfx);
  ~Text();

  void draw_text(const std::string& text, float x, float y, const std::string& font_name = "default",  float scale = 1.0f, float space_multiplier = 0.4f);

  UVCoords calculate_uv_coords(size_t index, const LoadedFont& font) const;
  size_t get_char_index(char c, const std::string& charset) const;

  void load_font(const std::string& font_name);
  bool is_font_loaded(const std::string& font_name);

private:
  GFX& gfx;
  std::unordered_map<std::string, LoadedFont> font_cache;
};