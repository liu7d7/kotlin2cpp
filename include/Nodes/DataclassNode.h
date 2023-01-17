//
// Created by richard may clarkson on 13/01/2023.
//

#ifndef KT2CPP_DATACLASSNODE_H
#define KT2CPP_DATACLASSNODE_H

#include <vector>
#include "Node.h"
#include "Token.h"
#include "ArgNode.h"

class DataclassNode : public Node {
public:
  Token* idTok;
  std::vector<ArgNode*> ctor;
  std::vector<Token*> generics;

  DataclassNode(Token* idTok, std::vector<Token*> generics, std::vector<ArgNode*> args);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_DATACLASSNODE_H
