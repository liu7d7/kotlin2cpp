//
// Created by Aadi and Michael on 7/25/22.
//

#ifndef KT2CPP_VARACCESSNODE_H
#define KT2CPP_VARACCESSNODE_H

#include <vector>
#include <Nodes/Node.h>
#include <Token.h>
#include "Global.h"

class VarAccessNode : public Node {
public:
  Token* idTok;
  vector<Token*> members;
  Node* parent;
  Token* methodName;

  explicit VarAccessNode(Token* idTok, vector<Token*> members = {}, Node* parent = nullptr);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_VARACCESSNODE_H
