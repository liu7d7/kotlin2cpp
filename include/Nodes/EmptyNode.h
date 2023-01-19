//
// Created by richard may clarkson on 18/01/2023.
//

#ifndef KT2CPP_EMPTYNODE_H
#define KT2CPP_EMPTYNODE_H

#include "Node.h"
struct EmptyNode : Node {
  inline EmptyNode() : Node(N_EMPTY) {
    this->posStart = new Position(0, 0, 0, new std::string("EmptyNode"), new std::string("EmptyNode"));
    this->posEnd = new Position(0, 0, 0, new std::string("EmptyNode"), new std::string("EmptyNode"));
  }
  [[nodiscard]] inline std::string toString() const override {
    return "";
  }
};

#endif //KT2CPP_EMPTYNODE_H
