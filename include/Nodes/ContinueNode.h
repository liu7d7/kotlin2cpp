//
// Created by Aadi and Michael on 8/1/22.
//

#ifndef KT2CPP_CONTINUENODE_H
#define KT2CPP_CONTINUENODE_H

#include <Nodes/Node.h>

class ContinueNode : public Node {
public:
  ContinueNode(Position* posStart, Position* posEnd);

  std::string toString() const override;
};

#endif //KT2CPP_CONTINUENODE_H
