//
// Created by Aadi and Michael on 8/13/22.
//

#include <Nodes/IndexNode.h>

IndexNode::IndexNode(Node* item, Node* idx, Node* newVal) : Node(N_IDX) {
  this->item = item;
  this->idx = idx;
  this->newVal = newVal;

  this->posStart = item->posStart;
  this->posEnd = newVal ? newVal->posEnd : idx->posEnd;
}

std::string IndexNode::toString() const {
  return "(" + item->toString() + " [" + idx->toString() + "] " + (newVal ? newVal->toString() : "") + ")";
}