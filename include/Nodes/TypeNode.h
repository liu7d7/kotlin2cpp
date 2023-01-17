//
// Created by richard may clarkson on 14/01/2023.
//

#ifndef KT2CPP_TYPENODE_H
#define KT2CPP_TYPENODE_H

#include <vector>
#include "Node.h"
#include "Token.h"

class TypeNode : public Node {
public:
  Token* idTok;
  std::vector<TypeNode*> generics;

  explicit TypeNode(Token* typeTok, std::vector<TypeNode*> generics);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_TYPENODE_H
