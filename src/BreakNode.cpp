//
// Created by Aadi and Michael on 8/1/22.
//

#include <Nodes/BreakNode.h>

BreakNode::BreakNode(Position* posStart, Position* posEnd) : Node(N_BREAK) {
  this->posStart = posStart;
  this->posEnd = posEnd;
}

string BreakNode::toString() const {
  return "break";
}