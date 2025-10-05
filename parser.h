#pragma once

#include "node.h"
#include <iostream>
#include <map>
#include <vector>

class Parser {
public:
  static std::map<std::string, std::string> ent_map;

public:
  static std::string get_ent(std::string);
  static std::vector<Node> parse_html(const std::string &buff);
};
