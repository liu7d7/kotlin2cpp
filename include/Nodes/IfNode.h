//
// Created by Aadi and Michael on 7/26/22.
//

#ifndef KT2CPP_IFNODE_H
#define KT2CPP_IFNODE_H

#include <vector>
#include <Nodes/Node.h>

class IfNode : public Node {
public:
  std::vector<std::pair<Node*, Node*>> cases;
  Node* elseCase;

  IfNode(std::vector<std::pair<Node*, Node*>> cases, Node* elseCase);

  std::string toString() const override;
};

#endif //KT2CPP_IFNODE_H
