//
// Created by Aadi and Michael on 7/27/22.
//

#ifndef AT_CALLNODE_H
#define AT_CALLNODE_H

#include <vector>
#include <Token.h>
#include <Nodes/Node.h>

class CallNode : public Node {
public:
  Node* toCall;
  vector<Node*> args;

  CallNode(Node* toCall, vector<Node*> args);

  string toString() const override;
};

#endif //AT_CALLNODE_H
