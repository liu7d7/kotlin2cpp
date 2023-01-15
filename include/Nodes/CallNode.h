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
  vector<Node*> args;
  vector<TypeNode*> generics;

  CallNode(Node* toCall, vector<TypeNode*> generics, vector<Node*> args);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_CALLNODE_H
