//
// Created by Aadi and Michael on 7/29/22.
//

#ifndef AT_STRINGNODE_H
#define AT_STRINGNODE_H

#include <Token.h>
#include <Nodes/Node.h>

class StringNode : public Node {
public:
  Token* token;

  StringNode(Token* token);

  string toString() const override;
};

#endif //AT_STRINGNODE_H
