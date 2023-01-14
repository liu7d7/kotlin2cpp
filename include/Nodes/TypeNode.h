//
// Created by richard may clarkson on 14/01/2023.
//

#ifndef AT_TYPENODE_H
#define AT_TYPENODE_H

#include <vector>
#include "Node.h"
#include "Token.h"

class TypeNode : public Node {
public:
  Token* typeTok;
  vector<Token*> generics;

  explicit TypeNode(Token* typeTok, vector<Token*> generics);

  [[nodiscard]] string toString() const override;
};

#endif //AT_TYPENODE_H
