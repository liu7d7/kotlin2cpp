//
// Created by richard may clarkson on 16/01/2023.
//

#ifndef KT2CPP_INTERPOLATEDSTRINGNODE_H
#define KT2CPP_INTERPOLATEDSTRINGNODE_H

#include <vector>
#include "Node.h"

class InterpolatedStringNode : public Node {
public:
  std::vector<Node*> items;

  explicit InterpolatedStringNode(std::vector<Node*> items);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_INTERPOLATEDSTRINGNODE_H
