//
// Created by Aadi and Michael on 7/30/22.
//

#ifndef KT2CPP_LISTNODE_H
#define KT2CPP_LISTNODE_H

#include <vector>
#include <Nodes/Node.h>

class ListNode : public Node {
public:
  std::vector<Node*> nodes;

  ListNode(std::vector<Node*> nodes, Position* posStart, Position* posEnd);

  std::string toString() const override;
};

#endif //KT2CPP_LISTNODE_H
