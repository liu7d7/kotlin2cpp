//
// Created by richard may clarkson on 12/01/2023.
//
#include "Nodes/VarDeclNode.h"

VarDeclNode::VarDeclNode(Token* idTok, TypeNode* type, Node* value, vector<Token*> members) : Node(N_VAR_DECL) {
  this->idTok = idTok;
  this->typeNode = type;
  this->value = value;
  this->members = members;

  this->posStart = idTok->posStart;
  if (!members.empty()) {
    this->posEnd = members.back()->posEnd;
  } else {
    this->posEnd = value->posEnd;
  }
}

string VarDeclNode::toString() const {
  return idTok->toString() + " = " + value->toString();
}

