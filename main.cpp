
#include "includes/raylib.h"
#include "parser.h"
#include <cstdio>
#include <fstream>
#include <ostream>
#include <sstream>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "includes/raygui.h"

void wrap_content(std::string &content, const Font &font, float font_size,
                  float spacing, float max_width) {
  std::vector<std::string> outLines;
  std::istringstream ss(content);
  std::string paragraph;

  std::vector<std::string> paragraphs;
  {
    std::stringstream s2(content);
    std::string line;
    while (std::getline(s2, line)) {
      paragraphs.push_back(line);
    }
  }

  for (size_t p = 0; p < paragraphs.size(); ++p) {
    std::string &para = paragraphs[p];
    if (para.empty()) {
      continue;
    }

    std::istringstream ws(para);
    std::string word;
    std::string curLine;
    while (ws >> word) {
      std::string tryLine = curLine.empty() ? word : (curLine + " " + word);
      Vector2 size = MeasureTextEx(font, tryLine.c_str(), font_size, spacing);
      if (size.x <= max_width) {
        curLine = tryLine;
      } else {
        if (!curLine.empty()) {
          outLines.push_back(curLine);
        }
        curLine = word;
        Vector2 singleWordSize =
            MeasureTextEx(font, curLine.c_str(), font_size, spacing);
        if (singleWordSize.x > max_width) {
          outLines.push_back(curLine);
          curLine.clear();
        }
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
  content = wrapped;
}

int main(int argc, char **argv) {
  const int screenWidth = 800;
  const int screenHeight = 600;
  float font_size = 20.0f;
  float font_spacing = 1.5f;

  Color background_color = BLUE;
  std::string filename = "test.html";
  if (argc == 2) {
    filename = argv[1];
  }
  std::ifstream in(filename);
  if (!in) {
    InitWindow(screenWidth, screenHeight, "HTML Renderer - missing input");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(background_color);
      DrawText("Could not open test.html", 20, 20, 20, RED);
      EndDrawing();
    }
    CloseWindow();
    return 1;
  }

  std::stringstream ss;
  ss << in.rdbuf();
  std::string content(ss.str());
  content = Parser::parse_html(content);
  std::string title;
  title = "HTML Renderer - " + filename;
  InitWindow(screenWidth, screenHeight, title.c_str());

  Font font = GetFontDefault();
  int paddingTop = 10;
  int paddingLeft = 10;
  Rectangle panelRec = {0, 0, (float)screenWidth, (float)screenHeight};

  float availableWidth = panelRec.width - paddingLeft - 10.0f;
  wrap_content(content, font, font_size, font_spacing, availableWidth);
  Vector2 total_size =
      MeasureTextEx(font, content.c_str(), font_size, font_spacing);

  Rectangle panelContentRec = {0, 0, panelRec.width, total_size.y + paddingTop};
  Rectangle panelView = {0, 0, 0, 0};
  Vector2 panelScroll = {0, 0};
  bool showContentArea = true;

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(background_color);

    GuiScrollPanel(panelRec, NULL, panelContentRec, &panelScroll, &panelView);

    Vector2 drawRec;
    drawRec.x = panelRec.x + panelScroll.x + paddingLeft;
    drawRec.y = panelRec.y + panelScroll.y + paddingTop;

    BeginScissorMode((int)panelRec.x, (int)panelRec.y, (int)panelRec.width,
                     (int)panelRec.height);

    DrawTextEx(font, content.c_str(), drawRec, font_size, font_spacing, BLACK);
    EndScissorMode();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
