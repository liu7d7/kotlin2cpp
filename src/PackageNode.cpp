//
// Created by Aadi and Michael on 8/2/22.
//

#include <utility>
#include <vector>
#include <Nodes/PackageNode.h>

PackageNode::PackageNode(std::vector<Token*> nameTok, Node* body) : Node(N_PACKAGE) {
  this->idToks = std::move(nameTok);
  this->body = body;

  this->posStart = this->idToks.front()->posStart;
  this->posEnd = this->body->posEnd;
}

std::string PackageNode::toString() const {
  return "<Package: " + idToks.front()->toString() + ">";
}