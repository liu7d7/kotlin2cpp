//
// Created by Aadi and Michael on 8/3/22.
//

#ifndef KT2CPP_CLASSNODE_H
#define KT2CPP_CLASSNODE_H

#include <Token.h>
#include <Nodes/Node.h>
#include "ArgNode.h"
#include <vector>

class ClassNode : public Node {
public:
  Token* idTok;
  std::vector<ArgNode*> ctor;
  std::vector<Token*> generics;
  Node* body;
  Node* super;

  ClassNode(Token* type, std::vector<Token*> generics, std::vector<ArgNode*> ctor, Node* body, Node* super = nullptr);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_CLASSNODE_H
