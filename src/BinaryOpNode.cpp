//
// Created by Aadi and Michael on 7/25/22.
//

#include <Nodes/BinaryOpNode.h>

BinaryOpNode::BinaryOpNode(Token* opTok, Node* left, Node* right) : Node(N_BIN_OP) {
  this->left = left;
  this->right = right;
  this->opTok = opTok;

  this->posStart = left->posStart;
  this->posEnd = right->posEnd;
}

std::string BinaryOpNode::toString() const {
  return "(" + left->toString() + " " + opTok->value + " " + right->toString() + ")";
}