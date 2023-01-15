//
// Created by Aadi and Michael on 7/28/22.
//

#include <Nodes/WhileNode.h>

WhileNode::WhileNode(Node* condition, Node* body, bool doWhile) : Node(doWhile ? N_DO_WHILE : N_WHILE) {
  this->condition = condition;
  this->body = body;

  posStart = condition->posStart;
  posEnd = body->posEnd;
}

string WhileNode::toString() const {
  return "while " + condition->toString() + " " + body->toString();
}