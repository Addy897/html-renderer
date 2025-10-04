#include <memory>
#include <string>
#include <vector>
enum TAGS {
  START_TAG,
  END_TAG,
  TEXT_NODE,
};
class Node {
public:
  std::string m_data;
  TAGS m_type;

public:
  Node() {}
  Node(TAGS type);
  Node(TAGS type, std::string &val);
  bool is_valid();
};
