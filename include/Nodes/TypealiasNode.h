//
// Created by richard may clarkson on 14/01/2023.
//

#ifndef KT2CPP_TYPEALIASNODE_H
#define KT2CPP_TYPEALIASNODE_H

#include "Node.h"
#include "TypeNode.h"

class TypealiasNode : public Node {
public:
  TypeNode* from;
  TypeNode* to;

  TypealiasNode(TypeNode* from, TypeNode* to);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_TYPEALIASNODE_H
