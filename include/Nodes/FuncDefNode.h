//
// Created by Aadi and Michael on 7/27/22.
//

#ifndef KT2CPP_FUNCDEFNODE_H
#define KT2CPP_FUNCDEFNODE_H

#include <vector>
#include <Token.h>
#include <Nodes/Node.h>
#include "ArgNode.h"

class FuncDefNode : public Node {
public:
  Token* idTok;
  TypeNode* returnType;
  vector<ArgNode*> args;
  vector<Token*> generics;
  Node* body;
  bool autoReturn;

  FuncDefNode(Token* nameTok, TypeNode* returnType, vector<Token*> generics, vector<ArgNode*> args, Node* body, bool autoReturn);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_FUNCDEFNODE_H
