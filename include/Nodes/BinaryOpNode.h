//
// Created by Aadi and Michael on 7/25/22.
//

#ifndef AT_BINARYOPNODE_H
#define AT_BINARYOPNODE_H

#include <Nodes/Node.h>
#include <Token.h>

class BinaryOpNode : public Node {
public:
  Node* left, * right;
  Token* opTok;

  BinaryOpNode(Token* opTok, Node* left, Node* right);

  [[nodiscard]] string toString() const override;
};

#endif //AT_BINARYOPNODE_H
