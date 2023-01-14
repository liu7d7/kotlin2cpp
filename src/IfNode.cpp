//
// Created by Aadi and Michael on 7/26/22.
//

#include <Nodes/IfNode.h>

IfNode::IfNode(vector<pair<Node*, Node*>> cases, Node* elseCase) : Node(N_IF) {
  this->cases = cases;
  this->elseCase = elseCase;

  this->posStart = this->cases[0].first->posStart;
  if (this->elseCase != nullptr) {
    this->posEnd = this->elseCase->posEnd;
  } else {
    this->posEnd = this->cases.back().second->posEnd;
  }
}

string IfNode::toString() const {
  string casesStr = "";
  for (auto& p : this->cases) {
    casesStr += "case " + p.first->toString() + ": " + p.second->toString() + "\n";
  }
  if (this->elseCase != nullptr) {
    casesStr += "else: " + this->elseCase->toString() + "\n";
  }
  return "if (" + casesStr + ")";
}