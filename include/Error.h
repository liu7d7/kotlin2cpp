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
  std::string name, message;

  Error(Position* posStart, Position* posEnd, std::string name, std::string message);
  Error(Token* tok, std::string name, std::string message);

  [[nodiscard]] virtual std::string toString();

  [[nodiscard]] std::string addArrows(const std::string& text) const;
};

#endif //KT2CPP_ERROR_H
