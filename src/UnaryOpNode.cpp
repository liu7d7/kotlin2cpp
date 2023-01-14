//
// Created by Aadi and Michael on 7/25/22.
//

#include <Nodes/UnaryOpNode.h>

UnaryOpNode::UnaryOpNode(Token* opTok, Node* node) : Node(N_UN_OP) {
  this->opTok = opTok;
  this->node = node;
  this->posStart = opTok->posStart;
  this->posEnd = node->posEnd;
}

string UnaryOpNode::toString() const {
  return "(" + opTok->toString() + node->toString() + ")";
}