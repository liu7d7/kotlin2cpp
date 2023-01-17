//
// Created by Aadi and Michael on 7/28/22.
//

#ifndef KT2CPP_WHILENODE_H
#define KT2CPP_WHILENODE_H

#include <Nodes/Node.h>

class WhileNode : public Node {
public:
  Node* condition;
  Node* body;

  WhileNode(Node* condition, Node* body, bool doWhile);

  [[nodiscard]] std::string toString() const override;
};

#endif //KT2CPP_WHILENODE_H
