//
// Created by richard may clarkson on 14/01/2023.
//

#ifndef KT2CPP_TRANSPILER_H
#define KT2CPP_TRANSPILER_H

#include <unordered_map>
#include "stack"
#include "ParseResult.h"
#include "unordered_set"

enum TranslateLevel {
  TL_NONE, TL_IDENTIFIERS, TL_KT_UTIL
};

class Transpiler {
public:
  explicit Transpiler(ParseResult* ast);

  [[nodiscard]] string transpile();
  [[nodiscard]] string translateId(const string& in);
  [[nodiscard]] string buildKtUtils();

  void recurse(Node* in, stringstream& output, int nesting = 0);
private:
  ParseResult* ast;
  stringstream out;
  unordered_set<string> imports;
  unordered_set<string> ktUtils;
  stack<TranslateLevel> translationLevel;
  stack<NodeType> typeAbove;
};

#endif //KT2CPP_TRANSPILER_H
