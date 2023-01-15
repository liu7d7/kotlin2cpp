//
// Created by Aadi and Michael on 7/26/22.
//

#ifndef KT2CPP_FORNODE_H
#define KT2CPP_FORNODE_H

#include <Nodes/Node.h>
#include <Token.h>
#include "ArgNode.h"

class ForNode : public Node {
public:
  ArgNode* var;
  Node* iterable;
  Node* body;

  ForNode(ArgNode* var, Node* iterable, Node* body);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_FORNODE_H
