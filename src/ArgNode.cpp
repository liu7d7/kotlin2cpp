//
// Created by richard may clarkson on 13/01/2023.
//
#include "Nodes/ArgNode.h"
#include "Nodes/TypeNode.h"

ArgNode::ArgNode(Token* idTok, TypeNode* type, Node* value) : Node(N_ARG) {
  this->idTok = idTok;
  this->typeNode = type;
  this->defaultValue = value;

  this->posStart = idTok->posStart;
  if (value) {
    this->posEnd = value->posEnd;
  } else {
    this->posEnd = typeNode->posEnd;
  }
}

string ArgNode::toString() const {
  return idTok->toString() + ": " + typeNode->toString();
}

ArgNode::ArgNode() : Node(N_ARG) {

}

