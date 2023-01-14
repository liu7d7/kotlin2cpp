//
// Created by Preeti Thorat on 9/2/22.
//

#ifndef AT_MAPNODE_H
#define AT_MAPNODE_H

#include <unordered_map>
#include <Nodes/Node.h>

class MapNode : public Node {
public:
  unordered_map<Node*, Node*> elements;

  MapNode(unordered_map<Node*, Node*> elements, Position* posStart, Position* posEnd);

  string toString() const override;
};

#endif //AT_MAPNODE_H
