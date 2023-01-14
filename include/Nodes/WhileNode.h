//
// Created by Aadi and Michael on 7/28/22.
//

#ifndef AT_WHILENODE_H
#define AT_WHILENODE_H

#include <Nodes/Node.h>

class WhileNode : public Node {
public:
  Node* condition;
  Node* body;
  bool doWhile;

  WhileNode(Node* condition, Node* body, bool doWhile);

  [[nodiscard]] string toString() const override;
};

#endif //AT_WHILENODE_H
