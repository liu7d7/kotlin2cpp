//
// Created by Aadi and Michael on 7/25/22.
//

#include <Nodes/VarAssignNode.h>

VarAssignNode::VarAssignNode(Token* idTok, Node* value, std::vector<Token*> members) : Node(N_VAR_ASSIGN) {
  this->idTok = idTok;
  this->value = value;
  this->members = members;

  this->posStart = idTok->posStart;
  if (!members.empty()) {
    this->posEnd = members.back()->posEnd;
  } else {
    this->posEnd = value->posEnd;
  }
}

std::string VarAssignNode::toString() const {
  return idTok->toString() + " = " + value->toString();
}