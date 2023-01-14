//
// Created by richard may clarkson on 13/01/2023.
//

#ifndef AT_DATACLASSNODE_H
#define AT_DATACLASSNODE_H

#include <vector>
#include "Node.h"
#include "Token.h"
#include "ArgNode.h"

class DataclassNode : public Node {
public:
  Token* idTok;
  vector<ArgNode*> ctor;
  vector<Token*> generics;

  DataclassNode(Token* idTok, vector<Token*> generics, vector<ArgNode*> args);

  [[nodiscard]] string toString() const override;
};

#endif //AT_DATACLASSNODE_H
