//
// Created by Aadi and Michael on 7/27/22.
//

#include <utility>

#include <Nodes/FuncDefNode.h>

FuncDefNode::FuncDefNode(Token* nameTok, TypeNode* returnType, vector<Token*> generics, vector<ArgNode*> args, Node* body, bool autoReturn) : Node(N_FUNC_DEF) {
  this->idTok = nameTok;
  this->returnType = returnType;
  this->args = std::move(args);
  this->generics = std::move(generics);
  this->body = body;
  this->autoReturn = autoReturn;

  if (nameTok) {
    posStart = nameTok->posStart;
  } else if (!this->args.empty()) {
    posStart = this->args[0]->posStart;
  } else {
    posStart = body->posStart;
  }

  posEnd = body ? body->posEnd : returnType ? returnType->posEnd : args.empty() ? nameTok->posEnd : args.back()->posEnd;
}

string FuncDefNode::toString() const {
  string str = "Function " + idTok->value + "(";
  for (int i = 0; i < args.size(); i++) {
    str += args[i]->idTok->toString();
    if (i != args.size() - 1) {
      str += ", ";
    }
  }
  str += ") " + body->toString();
  return str;
}