//
// Created by richard may clarkson on 13/01/2023.
//
#include <utility>

#include "Nodes/DataclassNode.h"

DataclassNode::DataclassNode(Token* idTok, vector<Token*> generics, vector<ArgNode*> args) : Node(N_DATACLASS) {
  this->idTok = idTok;
  this->generics = std::move(generics);
  this->ctor = std::move(args);

  this->posStart = idTok->posStart;
  this->posEnd = this->ctor.back()->posEnd;
}

string DataclassNode::toString() const {
  return "DataclassNode::toString not implemented";
}

