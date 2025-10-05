#pragma once
#include "includes/raylib.h"
#include "parser.h"

#include <cstdio>
#include <format>
#include <fstream>
#include <ostream>
#include <sstream>
#include <vector>
#define FONT_SIZE 20.0f
#define FONT_SPACE 1.5f

class Renderer {
private:
  const int m_screen_width = 800;
  const int m_screen_height = 600;

  Font m_font;
  float m_font_size = FONT_SIZE;
  float m_font_spacing = FONT_SPACE;

  std::string m_content;
  std::string m_title;
  std::vector<Node> m_dom;

  Color m_background_color;

private:
  struct Properties {
    float font_size = FONT_SIZE;
    float font_space = FONT_SPACE;
    float line_height = font_size * 1.5;
    Color color = BLACK;
    Font font;
    float max_width;
    bool is_underline = false;
    bool draw = true;
  };

private:
  void init();
  void wrap_content(float font_size, float spacing, float max_width);
  float draw_dom(Vector2 draw_rec, bool);
  void load_all_codepoints(Font *font, const char *fontPath);
  void draw_word(const std::string &line, Vector2 &cursor, Vector2 &draw_rec,
                 const Properties &props);

public:
  Renderer();
  Renderer(std::vector<Node>);
  Renderer(std::vector<Node>, std::string &url);

  void render();
};
