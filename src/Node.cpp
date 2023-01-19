//
// Created by Aadi and Michael on 7/24/22.
//

#include <Nodes/Node.h>

std::string NodeType_toString(NodeType in) {
  return NodeType_strs[in];
}

Node::Node(NodeType type) {
  this->type = type;
}

std::string Node::toString() const {
  return std::to_string(type);
}