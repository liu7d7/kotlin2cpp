//
// Created by Aadi and Michael on 7/29/22.
//

#ifndef KT2CPP_STRINGNODE_H
#define KT2CPP_STRINGNODE_H

#include <Token.h>
#include <Nodes/Node.h>

class StringNode : public Node {
public:
  Token* token;

  StringNode(Token* token);

  string toString() const override;
};

#endif //KT2CPP_STRINGNODE_H
