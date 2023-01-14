//
// Created by Aadi and Michael on 8/13/22.
//

#ifndef AT_INDEXNODE_H
#define AT_INDEXNODE_H

#include <Nodes/Node.h>
#include "Global.h"

class IndexNode : public Node {
public:
  Node* item, * idx, * newVal;

  IndexNode(Node* item, Node* idx, Node* newVal = nullptr);

  [[nodiscard]] string toString() const override;
};

#endif //AT_INDEXNODE_H
