#include "node.h"

Node::Node(TAGS type) {
  m_type = type;
  m_data = "";
}

Node::Node(TAGS type, std::string &text) {
  m_type = type;
  m_data = text;
}
