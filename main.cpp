#include "parser.h"
#include "renderer.h"
int main(int argc, char **argv) {
  std::string filename = "test.html";
  if (argc == 2) {
    filename = argv[1];
  }
  Renderer *renderer;
  std::ifstream in(filename);
  if (!in) {
    renderer = new Renderer();
  } else {
    std::stringstream ss;
    ss << in.rdbuf();
    std::string content = ss.str();
    content = Parser::parse_html(content);
    renderer = new Renderer(content, filename);
  }
  renderer->render();
  delete renderer;
  return 0;
}
