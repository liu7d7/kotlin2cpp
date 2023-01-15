//
// Created by Aadi and Michael on 7/25/22.
//

#ifndef KT2CPP_PARSERESULT_H
#define KT2CPP_PARSERESULT_H

#include <Error.h>
#include <Nodes/Node.h>

class ParseResult {
public:
  Error* error;
  Node* node;
  int advanceCount, reverseCount{};

  ParseResult();

  Node* reg(ParseResult* res);

  Node* tryReg(ParseResult* res);

  void regAdvancement();

  ParseResult* success(Node* nodeIn);

  ParseResult* failure(Error* errorIn);
};

#endif //KT2CPP_PARSERESULT_H
