//
// Created by richard may clarkson on 14/01/2023.
//
#include <utility>

#include "Nodes/TypeNode.h"

TypeNode::TypeNode(Token* typeTok, vector<TypeNode*> generics) : Node(N_TYPE) {
this->idTok = typeTok;
  this->generics = std::move(generics);
  this->posStart = typeTok->posStart;
  this->posEnd = this->generics.empty() ? typeTok->posEnd : this->generics.back()->posEnd;
}

string TypeNode::toString() const {
  string result = idTok->value;
  if (!generics.empty()) {
    result += "<";
    for (auto& generic : generics) {
      result += generic->toString() + ", ";
    }
    result = result.substr(0, result.length() - 2) + ">";
  }
  return result;
}