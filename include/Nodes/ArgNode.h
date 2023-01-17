//
// Created by richard may clarkson on 13/01/2023.
//

#ifndef KT2CPP_ARGNODE_H
#define KT2CPP_ARGNODE_H

#include "Node.h"
#include "Token.h"
#include "Global.h"
#include "TypeNode.h"

class ArgNode : public Node {
public:
  Token* idTok;
  TypeNode* typeNode;
  Node* defaultValue;

  ArgNode();
  ArgNode(Token* idTok, TypeNode* typeTok, Node* value);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_ARGNODE_H
