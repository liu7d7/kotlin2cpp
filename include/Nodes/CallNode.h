//
// Created by Aadi and Michael on 7/27/22.
//

#ifndef KT2CPP_CALLNODE_H
#define KT2CPP_CALLNODE_H

#include <vector>
#include <Token.h>
#include <Nodes/Node.h>
#include "TypeNode.h"

class CallNode : public Node {
public:
  Node* toCall;
  std::vector<Node*> args;
  std::vector<TypeNode*> generics;

  CallNode(Node* toCall, std::vector<TypeNode*> generics, std::vector<Node*> args);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_CALLNODE_H
