//
// Created by richard may clarkson on 15/01/2023.
//
#include <utility>

#include "Nodes/LambdaNode.h"

string LambdaNode::toString() const {
  return "";
}

LambdaNode::LambdaNode(Node* body, vector<ArgNode*> args) : Node(N_LAMBDA), body(body), args(std::move(args)) {
  this->posStart = this->args.empty() ? body->posStart : this->args.front()->posStart;
  this->posEnd = body->posEnd;
}

