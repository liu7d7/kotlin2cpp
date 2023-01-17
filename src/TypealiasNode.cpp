//
// Created by richard may clarkson on 14/01/2023.
//

#include "Nodes/TypealiasNode.h"

TypealiasNode::TypealiasNode(TypeNode* from, TypeNode* to) : Node(N_TYPEALIAS), from(from), to(to) {
  this->posStart = from->posStart;
  this->posEnd = to->posEnd;
}

std::string TypealiasNode::toString() const {
  return "TypealiasNode";
}

