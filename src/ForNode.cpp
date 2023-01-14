//
// Created by Aadi and Michael on 7/26/22.
//

#include <Nodes/ForNode.h>

ForNode::ForNode(Token* varName, Node* iterable, Node* body, bool shouldReturn) : Node(N_FOR) {
  this->varName = varName;
  this->iterable = iterable;
  this->body = body;
  this->shouldReturn = shouldReturn;

  this->posStart = this->varName->posStart;
  this->posEnd = this->body->posEnd;
}

string ForNode::toString() const {
  return "for " + varName->toString() + " in " + iterable->toString() + " " + body->toString();
}