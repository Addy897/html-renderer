#include "parser.h"
using std::string;
string Parser::parse_html(string &content) {
  string text;
  bool in_tag = false;
  bool in_entity = false;
  bool is_tag_name = false;
  string entity;
  string tag_name;
  for (char &c : content) {
    if (c == '<') {
      in_tag = true;
      tag_name.clear();
      is_tag_name = true;
    } else if (c == '>') {
      in_tag = false;
    } else if (c == '&')
      in_entity = true;
    else if (c == ';' && in_entity) {
      in_entity = false;
      if (entity == "lt")
        text.push_back('<');
      else if (entity == "gt")
        text.push_back('>');
      else if (entity == "amp")
        text.push_back('&');
      entity.clear();
    } else if (!in_tag && tag_name != "style") {
      if (in_entity) {
        if (isalnum(c)) {
          entity.push_back(c);
          continue;
        } else {
          in_entity = false;
          text.append("&" + entity);
          entity.clear();
        }
      }
      text.push_back(c);
    } else {
      if (c == ' ')
        is_tag_name = false;
      if (is_tag_name)
        tag_name.push_back(c);
    }
  }
  return text;
}
