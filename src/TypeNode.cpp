//
// Created by richard may clarkson on 14/01/2023.
//
#include <utility>

#include "Nodes/TypeNode.h"

TypeNode::TypeNode(Token* typeTok, std::vector<TypeNode*> generics) : Node(N_TYPE) {
this->idTok = typeTok;
  this->generics = std::move(generics);
  this->posStart = typeTok->posStart;
  this->posEnd = this->generics.empty() ? typeTok->posEnd : this->generics.back()->posEnd;
}

std::string TypeNode::toString() const {
  std::string result = idTok->value;
  if (!generics.empty()) {
    result += "<";
    for (auto& generic : generics) {
      result += generic->toString() + ", ";
    }
    result = result.substr(0, result.length() - 2) + ">";
  }
  return result;
}