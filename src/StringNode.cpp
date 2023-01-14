//
// Created by Aadi and Michael on 7/29/22.
//

#include <Nodes/StringNode.h>

StringNode::StringNode(Token* token) : Node(N_STRING) {
  this->token = token;
  this->posStart = token->posStart;
  this->posEnd = token->posEnd;
}

string StringNode::toString() const {
  return token->value;
}