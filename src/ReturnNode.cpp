//
// Created by Aadi and Michael on 8/1/22.
//

#include <Nodes/ReturnNode.h>

ReturnNode::ReturnNode(Node* returnValue, Position* posStart, Position* posEnd) : Node(N_RETURN) {
  this->returnNode = returnValue;
  this->posStart = posStart;
  this->posEnd = posEnd;
}

std::string ReturnNode::toString() const {
  return "return " + returnNode->toString();
}