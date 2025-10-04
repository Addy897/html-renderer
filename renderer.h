#pragma once
#include "includes/raylib.h"
#include "parser.h"

#include <cstdio>
#include <format>
#include <fstream>
#include <ostream>
#include <sstream>
#include <vector>

class Renderer {
private:
  const int m_screen_width = 800;
  const int m_screen_height = 600;

  Font m_font;
  float m_font_size = 28.0f;
  float m_font_spacing = 1.5f;

  std::string m_content;
  std::string m_title;
  std::vector<Node> m_dom;

  Color m_background_color;

private:
  void init();
  void wrap_content(float font_size, float spacing, float max_width);
  float draw_dom(Vector2 draw_rec, bool);
  void load_all_codepoints(Font *font, const char *fontPath);

public:
  Renderer();
  Renderer(std::vector<Node>);
  Renderer(std::vector<Node>, std::string &url);

  void render();
};
