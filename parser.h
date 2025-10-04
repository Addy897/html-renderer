#pragma once

#include "node.h"
#include <iostream>
#include <map>
#include <vector>

class Parser {
public:
  static std::string parse_html(std::string &buff);
  static std::vector<Node> parse_html_to_dom(std::string &buff);
};
