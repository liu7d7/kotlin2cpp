//
// Created by Aadi and Michael on 7/25/22.
//

#include <ParseResult.h>

ParseResult::ParseResult() {
  this->error = nullptr;
  this->node = nullptr;
  advanceCount = 0;
}

Node* ParseResult::reg(ParseResult* res) {
  advanceCount += res->advanceCount;
  if (res->error) this->error = res->error;
  return res->node;
}

Node* ParseResult::tryReg(ParseResult* res) {
  if (res->error) {
    reverseCount = res->advanceCount;
    return nullptr;
  }
  return reg(res);
}

void ParseResult::regAdvancement() {
  advanceCount++;
}

ParseResult* ParseResult::failure(Error* errorIn) {
  if (!this->error || advanceCount == 0) this->error = errorIn;
  return this;
}

ParseResult* ParseResult::success(Node* nodeIn) {
  this->node = nodeIn;
  return this;
}
