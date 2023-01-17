//
// Created by Aadi and Michael on 8/2/22.
//

#ifndef KT2CPP_PACKAGENODE_H
#define KT2CPP_PACKAGENODE_H

#include <Token.h>
#include <vector>
#include <Nodes/Node.h>
#include "Global.h"

class PackageNode : public Node {
public:
  std::vector<Token*> idToks;
  Node* body;

  PackageNode(std::vector<Token*> nameTok, Node* body);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_PACKAGENODE_H
