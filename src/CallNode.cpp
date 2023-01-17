//
// Created by Aadi and Michael on 7/27/22.
//

#include <Nodes/CallNode.h>

#include <utility>

CallNode::CallNode(Node* toCall, std::vector<TypeNode*> generics, std::vector<Node*> args) : Node(N_CALL) {
  this->toCall = toCall;
  this->args = args;
  this->generics = std::move(generics);

  posStart = toCall->posStart;
  if (!args.empty()) {
    posEnd = args[args.size() - 1]->posEnd;
  } else {
    posEnd = toCall->posEnd;
  }
}

std::string CallNode::toString() const {
  std::string str = toCall->toString();
  str += "(";
  for (int i = 0; i < args.size(); i++) {
    str += args[i]->toString();
    if (i != args.size() - 1) {
      str += ", ";
    }
  }
  str += ")";
  return str;
}