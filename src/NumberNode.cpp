//
// Created by Aadi and Michael on 7/24/22.
//

#include <Nodes/NumberNode.h>

NumberNode::NumberNode(Token* token) : Node(NodeType::N_NUMBER) {
  this->token = token;
  this->value = stod(token->value);
  posStart = token->posStart;
  posEnd = token->posEnd;
}

std::string NumberNode::toString() const {
  return token->toString();
}