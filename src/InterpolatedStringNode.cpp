//
// Created by richard may clarkson on 16/01/2023.
//
#include "Nodes/InterpolatedStringNode.h"

InterpolatedStringNode::InterpolatedStringNode(std::vector<Node*> items) : Node(N_INTERPOLATED_STRING), items(std::move(items)) {}

std::string InterpolatedStringNode::toString() const {
  return {};
}

