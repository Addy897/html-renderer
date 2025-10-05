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

std::map<string, string> Parser::ent_map = {
    {"quot", "\""},  {"apos", "'"},   {"amp", "&"},    {"lt", "<"},
    {"gt", ">"},     {"nbsp", " "},   {"iexcl", "¡"},  {"cent", "¢"},
    {"pound", "£"},  {"curren", "¤"}, {"yen", "¥"},    {"brvbar", "¦"},
    {"sect", "§"},   {"uml", "¨"},    {"copy", "©"},   {"ordf", "ª"},
    {"laquo", "«"},  {"not", "¬"},    {"shy", "­"},    {"reg", "®"},
    {"macr", "¯"},   {"deg", "°"},    {"plusmn", "±"}, {"sup2", "²"},
    {"sup3", "³"},   {"acute", "´"},  {"micro", "µ"},  {"para", "¶"},
    {"middot", "·"}, {"cedil", "¸"},  {"sup1", "¹"},   {"ordm", "º"},
    {"raquo", "»"},  {"frac14", "¼"}, {"frac12", "½"}, {"frac34", "¾"},
    {"times", "×"},  {"divide", "÷"}, {"le", "≤"},     {"ge", "≥"},
    {"ne", "≠"},     {"equiv", "≡"},  {"infin", "∞"},  {"radic", "√"},
    {"hellip", "…"}, {"ndash", "–"},  {"#39", "\'"},
};

string Parser::get_ent(string ent_code) {
  auto it = ent_map.find(ent_code);
  if (it != ent_map.end()) {
    return it->second;
  }
  return "&" + ent_code + ";";
}
std::vector<Node> Parser::parse_html(const string &content) {

  std::vector<Node> tokens;
  State state = State::DATA;
  string buffer;
  string currentToken;
  string temp_ent;
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
        temp_ent.clear();
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
      if (isalnum(ch) || ch == '#') {
        temp_ent.push_back(ch);
      } else if (ch == ';') {
        temp_ent = get_ent(temp_ent);
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
        temp_ent.push_back(ch);
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
