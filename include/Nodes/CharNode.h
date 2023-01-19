//
// Created by richard may clarkson on 18/01/2023.
//

#ifndef KT2CPP_CHARNODE_H
#define KT2CPP_CHARNODE_H

#include "Node.h"
#include "Token.h"

struct CharNode : public Node {
  Token* token;

  inline CharNode(Token* token) : Node(N_CHAR), token(token) {}
  [[nodiscard]] inline std::string toString() const override { return token->value; };

};

#endif //KT2CPP_CHARNODE_H
