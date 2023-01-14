//
// Created by Aadi and Michael on 8/28/22.
//

#ifndef AT_IMPORTNODE_H
#define AT_IMPORTNODE_H

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

#endif //AT_IMPORTNODE_H
