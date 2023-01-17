//
// Created by Aadi and Michael on 7/30/22.
//

#include <Nodes/ListNode.h>

#include <utility>

ListNode::ListNode(std::vector<Node*> nodes, Position* posStart, Position* posEnd) : Node(N_LIST) {
  this->nodes = std::move(nodes);

  this->posStart = posStart;
  this->posEnd = posEnd;
}

std::string ListNode::toString() const {
  std::string str = "[";
  for (int i = 0; i < nodes.size(); i++) {
    str += nodes[i]->toString();
    if (i != nodes.size() - 1) {
      str += ", ";
    }
  }
  str += "]";
  return str;
}