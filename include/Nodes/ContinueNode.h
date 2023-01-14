//
// Created by Aadi and Michael on 8/1/22.
//

#ifndef AT_CONTINUENODE_H
#define AT_CONTINUENODE_H

#include <Nodes/Node.h>

class ContinueNode : public Node {
public:
  ContinueNode(Position* posStart, Position* posEnd);

  string toString() const override;
};

#endif //AT_CONTINUENODE_H
