//
// Created by Aadi and Michael on 8/3/22.
//

#include <Nodes/ClassNode.h>

ClassNode::ClassNode(Token* type, vector<Token*> generics, vector<ArgNode*> ctor, Node* body, Node* super) : Node(N_CLASS) {
  this->idTok = type;
  this->ctor = std::move(ctor);
  this->generics = std::move(generics);
  this->body = body;
  this->super = super;

  this->posStart = this->idTok->posStart;
  this->posEnd = this->body ? this->body->posEnd : this->ctor.empty() ? this->idTok->posEnd : this->ctor.back()->posEnd;
}

string ClassNode::toString() const {
  return "type " + this->idTok->toString() + " = " + this->body->toString();
}