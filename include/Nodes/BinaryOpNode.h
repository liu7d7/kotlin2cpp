//
// Created by Aadi and Michael on 7/25/22.
//

#ifndef KT2CPP_BINARYOPNODE_H
#define KT2CPP_BINARYOPNODE_H

#include <Nodes/Node.h>
#include <Token.h>

class BinaryOpNode : public Node {
public:
  Node* left, * right;
  Token* opTok;

  BinaryOpNode(Token* opTok, Node* left, Node* right);

  [[nodiscard]] string toString() const override;
};

#endif //KT2CPP_BINARYOPNODE_H
