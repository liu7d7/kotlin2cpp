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
  std::vector<ArgNode*> args;
  std::vector<Token*> generics;
  Node* body;
  bool autoReturn;

  FuncDefNode(Token* nameTok, TypeNode* returnType, std::vector<Token*> generics, std::vector<ArgNode*> args, Node* body, bool autoReturn);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_FUNCDEFNODE_H
