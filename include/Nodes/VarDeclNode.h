//
// Created by richard may clarkson on 12/01/2023.
//

#ifndef KT2CPP_VARDECLNODE_H
#define KT2CPP_VARDECLNODE_H

#include <vector>
#include "Node.h"
#include "Token.h"
#include "Global.h"
#include "TypeNode.h"

class VarDeclNode : public Node {
public:
  Token* idTok;
  TypeNode* typeNode;
  Node* value;
  std::vector<Token*> members;
  bool constant = false;

  VarDeclNode(Token* idTok, TypeNode* type, Node* value, std::vector<Token*> members = {});

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_VARDECLNODE_H
