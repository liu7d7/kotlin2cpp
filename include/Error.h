//
// Created by Aadi and Michael on 7/24/22.
//

#ifndef KT2CPP_ERROR_H
#define KT2CPP_ERROR_H

#include <vector>
#include <sstream>
#include <Position.h>
#include "Global.h"
#include "Token.h"

class Error {
public:
  Position* posStart, * posEnd;
  string name, message;

  Error(Position* posStart, Position* posEnd, string name, string message);
  Error(Token* tok, string name, string message);

  [[nodiscard]] virtual string toString();

  [[nodiscard]] string addArrows(const string& text) const;
};

#endif //KT2CPP_ERROR_H
