//
// Created by Aadi and Michael on 8/1/22.
//

#include <Nodes/ContinueNode.h>

ContinueNode::ContinueNode(Position* posStart, Position* posEnd) : Node(N_CONTINUE) {
  this->posStart = posStart;
  this->posEnd = posEnd;
}

std::string ContinueNode::toString() const {
  return "continue";
}