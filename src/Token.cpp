//
// Created by Aadi and Michael on 7/23/22.
//

#include <Token.h>

#include <utility>

string TokenType_toString(TokenType type) {
  const string strs[] {
    "NUMBER", "STRING", "IDENTIFIER", "END_OF_FILE", "NEWLINE", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "FUN", "LPAREN", "RPAREN", "LCURLYBRACE", "RCURLYBRACE", "COLON",
    "COMMA", "RETURN", "CLASS", "PACKAGE", "MODULO", "LESS_THAN", "GREATER_THAN", "EQUAL", "NOT_EQUAL", "LESS_THAN_EQUAL", "GREATER_THAN_EQUAL", "AND", "OR", "NOT", "IF",
    "ARROW", "LBRACKET", "RBRACKET", "DOT", "CONTINUE", "BREAK", "LOOP", "IMPORT", "DATA", "IN", "AS", "TRUE", "FALSE", "DO", "NUL", "MODULO_EQ", "VAR", "FOR", "WHILE",
    "ELSE", "BIT_AND", "BIT_OR", "BIT_XOR", "NUL_SAFE", "NUL_TERNARY", "ASSIGN", "PLUS_EQ", "MINUS_EQ", "MULTIPLY_EQ", "DIVIDE_EQ", "INCREMENT", "DECREMENT", "RANGE"
  };
  return strs[type];
}

Token::Token(TokenType type, string value, Position* posStart, Position* posEnd) {
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

string Token::toString() const {
  string s = TokenType_toString(type) + ": " + value;
  if (posStart) {
    s += "(" + to_string(posStart->line) + "," + to_string(posStart->col);
  }

  if (posEnd) {
    s += ":" + to_string(posEnd->line) + "," + to_string(posEnd->col) + ")";
  } else {
    s += ")";
  }
  return s;
}

