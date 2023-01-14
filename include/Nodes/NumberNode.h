//
// Created by Aadi and Michael on 7/24/22.
//

#ifndef AT_NUMBERNODE_H
#define AT_NUMBERNODE_H

#include <Token.h>
#include <Nodes/Node.h>

class NumberNode : public Node {
public:
  double value;
  Token* token;

  NumberNode(Token* token);

  string toString() const override;
};

#endif //AT_NUMBERNODE_H
