//
// Created by Preeti Thorat on 9/2/22.
//

#include <Nodes/MapNode.h>

MapNode::MapNode(unordered_map<Node*, Node*> elements, Position* posStart, Position* posEnd) : Node(N_MAP) {
  this->elements = elements;
  this->posStart = posStart;
  this->posEnd = posEnd;
}

string MapNode::toString() const {
  string result = "{";
  for (auto const& [key, val] : elements) {
    result += key->toString() + ": " + val->toString() + ", ";
  }
  result += "}";
  return result;
}