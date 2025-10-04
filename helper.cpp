#include "node.h"
#include <iostream>
#include <vector>
void print_nodes(const std::vector<Node> &nodes) {
  int i = 0;
  for (Node n : nodes) {
    for (int j = 0; j < i; j++) {
      std::cout << " ";
    }
    std::cout << n.m_data << "\n";
    if (n.m_type == TAGS::START_TAG) {
      i++;
    } else if (n.m_type == TAGS::END_TAG) {
      i--;
      i--;
    }
  }
}
