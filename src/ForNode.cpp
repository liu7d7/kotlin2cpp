//
// Created by Aadi and Michael on 7/26/22.
//

#include <Nodes/ForNode.h>

ForNode::ForNode(ArgNode* varName, Node* iterable, Node* body) : Node(N_FOR) {
  this->var = varName;
  this->iterable = iterable;
  this->body = body;

  this->posStart = this->var->posStart;
  this->posEnd = this->body->posEnd;
}

std::string ForNode::toString() const {
  return "for " + var->toString() + " in " + iterable->toString() + " " + body->toString();
}