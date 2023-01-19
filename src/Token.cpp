//
// Created by Aadi and Michael on 7/23/22.
//

#include <Token.h>

#include <utility>

std::string TokenType_toString(TokenType type) {
  return TokenType_strs[type];
}

Token::Token(TokenType type, std::string value, Position* posStart, Position* posEnd) {
  this->type = type;
  this->value = std::move(value);

  if (posStart) {
    this->posStart = posStart->copy();
    this->posEnd = posStart->copy();
    this->posEnd->advance();
  }

  if (posEnd) {
    this->posEnd = posEnd->copy();
  }
}

std::string Token::toString() const {
  std::string s = TokenType_toString(type) + ": " + value;
  if (posStart) {
    s += "(" + std::to_string(posStart->line) + "," + std::to_string(posStart->col);
  }

  if (posEnd) {
    s += ":" + std::to_string(posEnd->line) + "," + std::to_string(posEnd->col) + ")";
  } else {
    s += ")";
  }
  return s;
}

