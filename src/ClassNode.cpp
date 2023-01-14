//
// Created by Aadi and Michael on 8/3/22.
//

#include <Nodes/ClassNode.h>

ClassNode::ClassNode(Token* type, vector<Token*> generics, vector<ArgNode*> ctor, Node* body, Node* super) : Node(N_CLASS) {
  this->nameTok = type;
  this->ctor = std::move(ctor);
  this->generics = std::move(generics);
  this->body = body;
  this->super = super;

  this->posStart = this->nameTok->posStart;
  this->posEnd = this->body ? this->body->posEnd : this->ctor.empty() ? this->nameTok->posEnd : this->ctor.back()->posEnd;
}

string ClassNode::toString() const {
  return "type " + this->nameTok->toString() + " = " + this->body->toString();
}