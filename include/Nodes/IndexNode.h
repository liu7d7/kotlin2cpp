//
// Created by Aadi and Michael on 8/13/22.
//

#ifndef KT2CPP_INDEXNODE_H
#define KT2CPP_INDEXNODE_H

#include <Nodes/Node.h>
#include "Global.h"

class IndexNode : public Node {
public:
  Node* item, * idx, * newVal;

  IndexNode(Node* item, Node* idx, Node* newVal = nullptr);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_INDEXNODE_H
