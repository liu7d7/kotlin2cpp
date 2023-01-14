//
// Created by Aadi and Michael on 7/25/22.
//

#include <Nodes/VarAccessNode.h>

VarAccessNode::VarAccessNode(Token* idTok, vector<Token*> members, Node* parent) : Node(N_VAR_ACCESS) {
  this->idTok = idTok;
  this->members = members;
  this->parent = parent;

  if (parent != nullptr) {
    this->posStart = parent->posStart;
    this->posEnd = members.back()->posEnd;
  } else {
    this->posStart = idTok->posStart;
    if (members.empty())
      this->posEnd = idTok->posEnd;
    else
      this->posEnd = members.back()->posEnd;
  }
}

string VarAccessNode::toString() const {
  return idTok->value;
}