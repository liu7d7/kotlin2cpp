//
// Created by Aadi and Michael on 7/24/22.
//

#ifndef KT2CPP_NUMBERNODE_H
#define KT2CPP_NUMBERNODE_H

#include <Token.h>
#include <Nodes/Node.h>

class NumberNode : public Node {
public:
  double value;
  Token* token;

  NumberNode(Token* token);

  std::string toString() const override;
};

#endif //KT2CPP_NUMBERNODE_H
