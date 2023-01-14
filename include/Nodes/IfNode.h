//
// Created by Aadi and Michael on 7/26/22.
//

#ifndef AT_IFNODE_H
#define AT_IFNODE_H

#include <vector>
#include <Nodes/Node.h>

class IfNode : public Node {
public:
  vector<pair<Node*, Node*>> cases;
  Node* elseCase;

  IfNode(vector<pair<Node*, Node*>> cases, Node* elseCase);

  string toString() const override;
};

#endif //AT_IFNODE_H
