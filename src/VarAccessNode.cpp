//
// Created by Aadi and Michael on 7/25/22.
//

#include <Nodes/VarAccessNode.h>

VarAccessNode::VarAccessNode(Token* idTok, std::vector<Token*> members, Node* parent) : Node(N_VAR_ACCESS) {
  this->idTok = idTok;
  this->members = members;
  this->parent = parent;

  if (parent != nullptr) {
    this->posStart = parent->posStart;
    if (members.empty()) {
      methodName = nullptr;
      this->posEnd = parent->posEnd;
    } else {
      this->posEnd = members.back()->posEnd;
      methodName = members.back();
    }
  } else {
    this->posStart = idTok->posStart;
    if (members.empty()) {
      this->posEnd = idTok->posEnd;
      methodName = idTok;
    } else {
      this->posEnd = members.back()->posEnd;
      methodName = members.back();
    }
  }
}

std::string VarAccessNode::toString() const {
  std::string str;
  if (parent != nullptr) {
    str += parent->toString();
  }
  if (idTok != nullptr) {
    str += idTok->value;
  }
  for (auto& member : members) {
    str += "." + member->value;
  }
  return str;
}