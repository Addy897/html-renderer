#include "renderer.h"
#include "includes/raylib.h"
#include "parser.h"
#include <sstream>
#include <string>
#define RAYGUI_IMPLEMENTATION
#include "includes/raygui.h"
void Renderer::load_all_codepoints(Font *font, const char *fontPath) {

  std::vector<int> codes = {
      0x0000, 0x007f, 0x0080, 0x00ff, 0x0900, 0x097F, 0x0600,
      0x06FF, 0x05D0, 0x05EA, 0x0400, 0x04FF, 0x0500, 0x052F,
      0x2DE0, 0x2DFF, 0xA640, 0xA69F, 0x0370, 0x03FF, 0x1F00,
      0x1FFF, 0x00C0, 0x017F, 0x0180, 0x024F, 0x2000, 0x206F,
  };
  int glyph_count = font->glyphCount;
  std::vector<int> allCodepoints;
  for (int i = 0; i < glyph_count; i++) {
    allCodepoints.push_back(font->glyphs[i].value);
  }
  for (size_t i = 0; i + 1 < codes.size(); i += 2) {
    int start = codes[i];
    int stop = codes[i + 1];

    for (int cp = start; cp <= stop; cp++) {
      allCodepoints.push_back(cp);
    }
  }

  UnloadFont(*font);
  *font = LoadFontEx(fontPath, 32, allCodepoints.data(), allCodepoints.size());
}
void Renderer::wrap_content(float font_size, float spacing, float max_width) {}
Renderer::Renderer() {
  m_content = "";
  m_title = "HTML Renderer - blank";
  init();
}

Renderer::Renderer(std::vector<Node> dom) {
  m_dom = dom;
  m_title = "HTML Renderer - ";

  init();
}
Renderer::Renderer(std::vector<Node> dom, std::string &url) {
  m_dom = dom;
  m_title = "HTML Renderer - " + url;

  init();
}
void Renderer::init() {
  InitWindow(m_screen_width, m_screen_height, m_title.c_str());
  SetTargetFPS(60);

  m_background_color = RAYWHITE;
  const char *font_path = "resources/NotoSans-Regular.ttf";
  m_font = LoadFont(font_path);
  load_all_codepoints(&m_font, font_path);
}
void Renderer::draw_word(const std::string &line, Vector2 &cursor,
                         Vector2 &draw_rec, const Properties &props) {
  std::stringstream word_stream(line);
  std::string word;
  while (word_stream >> word) {
    std::string temp = word + " ";
    Vector2 temp_size = MeasureTextEx(props.font, temp.c_str(), props.font_size,
                                      props.font_space);

    if (cursor.x + temp_size.x > draw_rec.x + props.max_width) {
      cursor.y += props.line_height;
      cursor.x = draw_rec.x;
    }

    if (props.draw) {
      DrawTextEx(props.font, temp.c_str(), cursor, props.font_size,
                 props.font_space, props.color);
      if (props.is_underline) {
        DrawLineEx({cursor.x, cursor.y + temp_size.y},
                   {cursor.x + temp_size.x, cursor.y + temp_size.y}, 1.0f,
                   props.color);
      }
    }

    cursor.x += temp_size.x;
  }
}
float Renderer::draw_dom(Vector2 draw_rec, bool draw) {
  Vector2 cursor = draw_rec;
  Properties props;
  props.font = m_font;
  props.max_width = m_screen_width - 20.0f - 10.0f;
  props.line_height = props.font_size * 1.5;
  props.draw = draw;
  bool skip = false;
  bool allow_new_line = false;
  for (const Node &node : m_dom) {
    props.line_height = props.font_size * 1.5f;
    switch ((int)node.m_type) {
    case TAGS::START_TAG: {
      if (node.m_data == "style" || node.m_data == "script" ||
          node.m_data == "title") {
        skip = true;
      }
      if (node.m_data == "p") {
        cursor.y += props.line_height + props.font_size;

        cursor.x = draw_rec.x;
      }
      if (node.m_data == "h1") {
        props.font_size = 28.0f;
      }
      if (node.m_data == "div" || node.m_data == "h1") {
        cursor.y += props.line_height;
        cursor.x = draw_rec.x;
      }
      if (node.m_data == "a") {
        props.color = BLUE;
        props.is_underline = true;
      }
      if (node.m_data == "code") {
        allow_new_line = true;
      }
      break;
    }
    case TAGS::END_TAG: {
      if (node.m_data == "script" || node.m_data == "style" ||
          node.m_data == "title") {
        skip = false;
      }
      if (node.m_data == "p") {
        cursor.y += m_font_size;
        cursor.x = draw_rec.x;
      }
      if (node.m_data == "h1") {
        props.font_size = m_font_size;
      }
      if (node.m_data == "a") {
        props.color = BLACK;
        props.is_underline = false;
      }
      allow_new_line = false;
      break;
    }
    case TAGS::TEXT_NODE: {

      if (skip)
        break;
      if (allow_new_line) {

        std::stringstream ss(node.m_data);
        std::string line;
        while (std::getline(ss, line, '\n')) {
          draw_word(line, cursor, draw_rec, props);
          cursor.y += props.line_height;
          cursor.x = draw_rec.x;
        }
        break;
      }
      draw_word(node.m_data, cursor, draw_rec, props);

      break;
    }
    }
  }
  return (cursor.y + props.line_height) - draw_rec.y;
}
void Renderer::render() {
  float padding_left = 10.0f;
  float padding_top = 10.0f;
  Rectangle panel_rec = {0, 0, (float)m_screen_width, (float)m_screen_height};
  float height = draw_dom(Vector2{padding_left, padding_top}, false);

  Rectangle panelContentRec = {0, 0, panel_rec.width, height};
  Rectangle panelView = {0, 0, 0, 0};
  Vector2 panelScroll = {0, 0};
  while (!WindowShouldClose()) {

    BeginDrawing();

    ClearBackground(m_background_color);
    GuiScrollPanel(panel_rec, NULL, panelContentRec, &panelScroll, &panelView);

    Vector2 draw_rec;
    draw_rec.x = panel_rec.x + panelScroll.x + padding_left;
    draw_rec.y = panel_rec.y + panelScroll.y + padding_top;

    BeginScissorMode((int)panel_rec.x, (int)panel_rec.y, (int)panel_rec.width,
                     (int)panel_rec.height);
    draw_dom(draw_rec, true);

    EndScissorMode();

    EndDrawing();
  }

  CloseWindow();
}
