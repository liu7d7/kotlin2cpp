//
// Created by richard may clarkson on 14/01/2023.
//

#ifndef AT_TRANSPILER_H
#define AT_TRANSPILER_H

#include <unordered_map>
#include "stack"
#include "ParseResult.h"

class Transpiler {
public:
  explicit Transpiler(ParseResult* ast);

  [[nodiscard]] string transpile();
  void recurse(Node* in, stringstream& output, int nesting = 0);
private:
  ParseResult* ast;
  stringstream out;
};

#endif //AT_TRANSPILER_H
