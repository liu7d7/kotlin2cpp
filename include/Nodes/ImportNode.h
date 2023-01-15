//
// Created by Aadi and Michael on 8/28/22.
//

#ifndef KT2CPP_IMPORTNODE_H
#define KT2CPP_IMPORTNODE_H

#include <Nodes/Node.h>
#include <Token.h>

class ImportNode : public Node {
public:
  Token* pkg = nullptr;
  Node* path = nullptr;

  ImportNode(Token* pkg);

  ImportNode(Node* path);

  string toString() const override;
};

#endif //KT2CPP_IMPORTNODE_H
