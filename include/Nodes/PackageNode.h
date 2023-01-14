//
// Created by Aadi and Michael on 8/2/22.
//

#ifndef AT_PACKAGENODE_H
#define AT_PACKAGENODE_H

#include <Token.h>
#include <vector>
#include <Nodes/Node.h>
#include "Global.h"

class PackageNode : public Node {
public:
  vector<Token*> nameToks;
  Node* body;

  PackageNode(vector<Token*> nameTok, Node* body);

  [[nodiscard]] string toString() const override;
};

#endif //AT_PACKAGENODE_H
