//
// Created by Aadi and Michael on 8/1/22.
//

#ifndef KT2CPP_BREAKNODE_H
#define KT2CPP_BREAKNODE_H

#include <Nodes/Node.h>
#include "Global.h"

class BreakNode : public Node {
public:
  BreakNode(Position* posStart, Position* posEnd);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_BREAKNODE_H
