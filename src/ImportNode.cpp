//
// Created by Aadi and Michael on 8/28/22.
//

#include <Nodes/ImportNode.h>

ImportNode::ImportNode(Token* pkg) : Node(N_IMPORT) {
  this->pkg = pkg;
  this->posStart = pkg->posStart;
  this->posEnd = pkg->posEnd;
}

ImportNode::ImportNode(Node* path) : Node(N_IMPORT) {
  this->path = path;
  // Modify position start to include '~+'
  this->posStart = path->posStart;
  this->posEnd = path->posEnd;
}

string ImportNode::toString() const {
  return "ImportNode: " + pkg->toString();
}