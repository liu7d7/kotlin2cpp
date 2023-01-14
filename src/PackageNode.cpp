//
// Created by Aadi and Michael on 8/2/22.
//

#include <utility>
#include <vector>
#include <Nodes/PackageNode.h>

PackageNode::PackageNode(vector<Token*> nameTok, Node* body) : Node(N_PACKAGE) {
  this->nameToks = std::move(nameTok);
  this->body = body;

  this->posStart = this->nameToks.front()->posStart;
  this->posEnd = this->body->posEnd;
}

string PackageNode::toString() const {
  return "<Package: " + nameToks.front()->toString() + ">";
}