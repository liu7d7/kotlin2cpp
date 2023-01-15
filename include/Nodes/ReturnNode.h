//
// Created by Aadi and Michael on 8/1/22.
//

#ifndef KT2CPP_RETURNNODE_H
#define KT2CPP_RETURNNODE_H

#include <Nodes/Node.h>

class ReturnNode : public Node {
public:
  Node* returnNode;

  ReturnNode(Node* returnValue, Position* posStart, Position* posEnd);

  string toString() const override;
};

#endif //KT2CPP_RETURNNODE_H
