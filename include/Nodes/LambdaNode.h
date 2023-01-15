//
// Created by richard may clarkson on 15/01/2023.
//

#ifndef KT2CPP_LAMBDANODE_H
#define KT2CPP_LAMBDANODE_H

#include <vector>
#include "Node.h"
#include "Token.h"
#include "ArgNode.h"

class LambdaNode : public Node {
public:
  Node* body;
  vector<ArgNode*> args;

  LambdaNode(Node* body, vector<ArgNode*> args);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_LAMBDANODE_H
