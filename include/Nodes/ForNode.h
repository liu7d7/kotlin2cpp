//
// Created by Aadi and Michael on 7/26/22.
//

#ifndef AT_FORNODE_H
#define AT_FORNODE_H

#include <Nodes/Node.h>
#include <Token.h>

class ForNode : public Node {
public:
  Token* varName;
  Node* iterable, * body;
  bool shouldReturn;

  ForNode(Token* varName, Node* iterable, Node* body, bool shouldReturn);

  string toString() const override;
};

#endif //AT_FORNODE_H
