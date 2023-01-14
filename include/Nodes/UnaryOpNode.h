//
// Created by Aadi and Michael on 7/25/22.
//

#ifndef AT_UNARYOPNODE_H
#define AT_UNARYOPNODE_H

#include <Nodes/Node.h>
#include <Token.h>

class UnaryOpNode : public Node {
public:
  Token* opTok;
  Node* node;

  UnaryOpNode(Token* opTok, Node* node);

  string toString() const override;
};

#endif //AT_UNARYOPNODE_H
