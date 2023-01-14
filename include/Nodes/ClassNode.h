//
// Created by Aadi and Michael on 8/3/22.
//

#ifndef AT_CLASSNODE_H
#define AT_CLASSNODE_H

#include <Token.h>
#include <Nodes/Node.h>
#include "ArgNode.h"
#include <vector>

class ClassNode : public Node {
public:
  Token* nameTok;
  vector<ArgNode*> ctor;
  vector<Token*> generics;
  Node* body;
  Node* super;

  ClassNode(Token* type, vector<Token*> generics, vector<ArgNode*> ctor, Node* body, Node* super = nullptr);

  [[nodiscard]] string toString() const override;
};

#endif //AT_CLASSNODE_H
