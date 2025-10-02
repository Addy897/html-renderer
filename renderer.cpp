#include "renderer.h"
#include "includes/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "includes/raygui.h"
void Renderer::add_codepoint_range(Font *font, const char *fontPath, int start,
                                   int stop) {
  int rangeSize = stop - start + 1;
  int currentRangeSize = font->glyphCount;

  int updatedCodepointCount = currentRangeSize + rangeSize;
  int *updatedCodepoints = (int *)RL_CALLOC(updatedCodepointCount, sizeof(int));

  for (int i = 0; i < currentRangeSize; i++)
    updatedCodepoints[i] = font->glyphs[i].value;

  for (int i = currentRangeSize; i < updatedCodepointCount; i++)
    updatedCodepoints[i] = start + (i - currentRangeSize);

  UnloadFont(*font);
  *font = LoadFontEx(fontPath, 32, updatedCodepoints, updatedCodepointCount);
  RL_FREE(updatedCodepoints);
}
void Renderer::wrap_content(float font_size, float spacing, float max_width) {
  std::vector<std::string> outLines;
  std::string paragraph;

  std::vector<std::string> paragraphs;
  {
    std::stringstream s2(m_content);
    std::string line;
    while (std::getline(s2, line)) {
      paragraphs.push_back(line);
    }
  }

  for (size_t p = 0; p < paragraphs.size(); ++p) {
    std::string &para = paragraphs[p];
    if (para.empty()) {
      outLines.push_back("\n");
      continue;
    }

    std::istringstream ws(para);
    std::string word;
    std::string curLine;
    while (ws >> word) {
      std::string tryLine = curLine.empty() ? word : (curLine + " " + word);

      Vector2 size = MeasureTextEx(m_font, tryLine.c_str(), font_size, spacing);
      if (size.x <= max_width) {
        curLine = tryLine;
      } else {
        Vector2 singleWordSize =
            MeasureTextEx(m_font, tryLine.c_str(), font_size, spacing);
        std::string temp;
        int i = 0;
        while (singleWordSize.x > max_width) {
          temp = tryLine.substr(0, tryLine.size() - i);
          singleWordSize =
              MeasureTextEx(m_font, temp.c_str(), font_size, spacing);
          i++;
        }
        outLines.push_back(temp);
        curLine = tryLine.substr(tryLine.size() - i, tryLine.size());
      }
    }
    if (!curLine.empty())
      outLines.push_back(curLine);
  }

  std::string wrapped;
  for (size_t i = 0; i < outLines.size(); ++i) {
    wrapped += outLines[i];
    if (i + 1 < outLines.size())
      wrapped += '\n';
  }
  m_content = wrapped;
}
Renderer::Renderer() {
  m_content = "";
  m_title = "HTML Renderer - blank";
  init();
}

Renderer::Renderer(std::string content) {
  m_content = content;
  m_title = "HTML Renderer - ";

  init();
}
Renderer::Renderer(std::string content, std::string &url) {
  m_content = content;
  m_title = "HTML Renderer - " + url;

  init();
}
void Renderer::init() {
  InitWindow(m_screen_width, m_screen_height, m_title.c_str());
  SetTargetFPS(60);

  m_background_color = RAYWHITE;
  m_font = LoadFont("resources/NotoSans-Regular.ttf");
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x900, 0x97f);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x600, 0x6ff);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x5d0, 0x5ea);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x400, 0x4ff);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x500, 0x52f);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x2de0,
                      0x2Dff);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0xa640,
                      0xA69f);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x370, 0x3ff);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x1f00,
                      0x1fff);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0xc0, 0x17f);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x180, 0x24f);
  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x2010,
                      0x206F);

  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x4E00,
                      0x9FFF);

  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x3040,
                      0x309F);

  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x30A0,
                      0x30FF);

  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0x31F0,
                      0x31FF);

  add_codepoint_range(&m_font, "resources/NotoSans-Regular.ttf", 0xAC00,
                      0xD7AF);
  float paddingLeft = 20.0f;
  float availableWidth = m_screen_width - paddingLeft - 10.0f;
  wrap_content(m_font_size, m_font_spacing, availableWidth);
}
void Renderer::render() {
  int paddingLeft = 10;
  int paddingTop = 10;
  Rectangle panelRec = {0, 0, (float)m_screen_width, (float)m_screen_height};

  Vector2 total_size =
      MeasureTextEx(m_font, m_content.c_str(), m_font_size, m_font_spacing);

  Rectangle panelContentRec = {0, 0, panelRec.width, total_size.y + paddingTop};
  Rectangle panelView = {0, 0, 0, 0};
  Vector2 panelScroll = {0, 0};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(m_background_color);

    GuiScrollPanel(panelRec, NULL, panelContentRec, &panelScroll, &panelView);

    Vector2 drawRec;
    drawRec.x = panelRec.x + panelScroll.x + paddingLeft;
    drawRec.y = panelRec.y + panelScroll.y + paddingTop;

    BeginScissorMode((int)panelRec.x, (int)panelRec.y, (int)panelRec.width,
                     (int)panelRec.height);

    DrawTextEx(m_font, m_content.c_str(), drawRec, m_font_size, m_font_spacing,
               BLACK);
    EndScissorMode();

    EndDrawing();
  }

  CloseWindow();
}
