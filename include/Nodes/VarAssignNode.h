//
// Created by Aadi and Michael on 7/25/22.
//

#ifndef KT2CPP_VARASSIGNNODE_H
#define KT2CPP_VARASSIGNNODE_H

#include <vector>
#include <Nodes/Node.h>
#include <Token.h>
#include "Global.h"

class VarAssignNode : public Node {
public:
  Token* idTok;
  Node* value;
  vector<Token*> members;
  bool constant = false;

  VarAssignNode(Token* idTok, Node* value, vector<Token*> members = {});

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_VARASSIGNNODE_H
