#include "parser.h"
using std::string;
enum class State {
  DATA,
  TAG_OPEN,
  TAG_NAME,
  END_TAG_OPEN,
  END_TAG_NAME,
  ATTRIBUTES,
  ENTITY,
  COMMENT
};

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
std::vector<Node> Parser::parse_html_to_dom(string &content) {

  std::vector<Node> tokens;
  State state = State::DATA;
  std::string buffer;
  std::string currentToken;
  std::string temp_ent;
  size_t i = 0;
  while (i < content.size()) {
    char ch = content[i];

    switch (state) {
    case State::DATA:
      if (ch == '<') {
        if (!buffer.empty()) {

          Node n(TAGS::TEXT_NODE, buffer);
          tokens.push_back(n);
          buffer.clear();
        }
        if (i + 1 < content.size() && content[i + 1] == '/') {
          state = State::END_TAG_OPEN;
        } else {
          state = State::TAG_OPEN;
        }
      } else if (ch == '&') {
        state = State::ENTITY;
      } else {
        buffer += ch;
      }
      break;

    case State::TAG_OPEN:
      if (isalpha(ch)) {
        currentToken = ch;
        state = State::TAG_NAME;
      } else if (ch == '!') {
        state = State::COMMENT;
      }
      break;
    case State::ENTITY:
      if (isalnum(ch)) {
        temp_ent.push_back(ch);
      } else if (ch == ';') {
        if (temp_ent == "gt")
          temp_ent = ">";
        else if (temp_ent == "lt")
          temp_ent = "<";
        else if (temp_ent == "amp")
          temp_ent = "&";
        else
          temp_ent = "&" + temp_ent + ";";
        buffer += temp_ent;
        state = State::DATA;
        temp_ent.clear();
      } else if (ch == '<') {
        if (!temp_ent.empty())
          buffer += temp_ent;
        if (!buffer.empty()) {

          Node n(TAGS::TEXT_NODE, buffer);
          tokens.push_back(n);
          buffer.clear();
        }
        if (i + 1 < content.size() && content[i + 1] == '/') {
          state = State::END_TAG_OPEN;
        } else {
          state = State::TAG_OPEN;
        }
        temp_ent.clear();
      } else {
        buffer += "&" + temp_ent;
        state = State::DATA;
        temp_ent.clear();
      }
      break;
    case State::TAG_NAME:
      if (isalnum(ch)) {
        currentToken += ch;
      } else if (ch == '>') {
        tokens.push_back(Node{TAGS::START_TAG, currentToken});
        currentToken.clear();
        state = State::DATA;
      } else if (ch == ' ') {
        state = State::ATTRIBUTES;
      }
      break;
    case State::ATTRIBUTES:
      if (ch == '>') {
        tokens.push_back(Node{TAGS::START_TAG, currentToken});
        currentToken.clear();
        state = State::DATA;
      } else if (ch == ' ') {
        state = State::ATTRIBUTES;
      }
      break;
    case State::END_TAG_OPEN:
      if (isalpha(ch)) {
        currentToken = ch;
        state = State::END_TAG_NAME;
      }
      break;

    case State::END_TAG_NAME:
      if (isalnum(ch)) {
        currentToken += ch;
      } else if (ch == '>') {
        tokens.push_back(Node{TAGS::END_TAG, currentToken});
        currentToken.clear();
        state = State::DATA;
      }
      break;

    case State::COMMENT:
      if (ch == '>') {
        state = State::DATA;
      }
      break;
    }

    i++;
  }

  if (!buffer.empty()) {
    Node n(TAGS::TEXT_NODE, buffer);

    tokens.push_back(n);
  }

  return tokens;
}
