//
// Created by Aadi and Michael on 7/23/22.
//

#ifndef KT2CPP_TOKEN_H
#define KT2CPP_TOKEN_H

#include <string>
#include <Position.h>
#include "Global.h"

enum TokenType {
  NUMBER,
  STRING,
  IDENTIFIER,
  END_OF_FILE,
  NEWLINE,
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  FUN,
  LPAREN,
  RPAREN,
  LCURLYBRACE,
  RCURLYBRACE,
  COLON,
  COMMA,
  RETURN,
  CLASS,
  PACKAGE,
  MODULO,
  LESS_THAN,
  GREATER_THAN,
  EQUAL,
  NOT_EQUAL,
  LESS_THAN_EQUAL,
  GREATER_THAN_EQUAL,
  AND,
  OR,
  NOT,
  IF,
  ARROW,
  LBRACKET,
  RBRACKET,
  DOT,
  CONTINUE,
  BREAK,
  LOOP,
  IMPORT,
  DATA,
  IN,
  AS,
  TRUE,
  FALSE,
  DO,
  NUL,
  MODULO_EQ,
  VAR,
  FOR,
  WHILE,
  ELSE,
  BIT_AND,
  BIT_OR,
  BIT_XOR,
  NUL_SAFE, // ?.
  NUL_TERNARY, // ?:
  ASSIGN,
  PLUS_EQ,
  MINUS_EQ,
  MULTIPLY_EQ,
  DIVIDE_EQ,
  INCREMENT,
  DECREMENT,
  RANGE,
  DOWNTO,
  STEP,
  UNTIL,
  TYPEALIAS,
  IS_BEGIN, // interpolated string
  IS_END,
  IS_EXPR_BEGIN,
  IS_EXPR_END,
  T_CHAR,
  INVALID,
};

const std::string TokenType_strs[] {
  "NUMBER", "STRING", "IDENTIFIER", "END_OF_FILE", "NEWLINE", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "FUN", "LPAREN", "RPAREN", "LCURLYBRACE", "RCURLYBRACE", "COLON",
  "COMMA", "RETURN", "CLASS", "PACKAGE", "MODULO", "LESS_THAN", "GREATER_THAN", "EQUAL", "NOT_EQUAL", "LESS_THAN_EQUAL", "GREATER_THAN_EQUAL", "AND", "OR", "NOT", "IF",
  "ARROW", "LBRACKET", "RBRACKET", "DOT", "CONTINUE", "BREAK", "LOOP", "IMPORT", "DATA", "IN", "AS", "TRUE", "FALSE", "DO", "NUL", "MODULO_EQ", "VAR", "FOR", "WHILE",
  "ELSE", "BIT_AND", "BIT_OR", "BIT_XOR", "NUL_SAFE", "NUL_TERNARY", "ASSIGN", "PLUS_EQ", "MINUS_EQ", "MULTIPLY_EQ", "DIVIDE_EQ", "INCREMENT", "DECREMENT", "RANGE", "DOWNTO", "STEP", "UNTIL", "TYPEALIAS",
  "IS_BEGIN", "IS_END", "IS_EXPR_BEGIN", "IS_EXPR_END", "T_CHAR", "INVALID"
};

[[nodiscard]] std::string TokenType_toString(TokenType type);

class Token {
public:
  std::string value;
  TokenType type;
  Position* posStart, * posEnd;

  Token(TokenType type, std::string value, Position* posStart = nullptr, Position* posEnd = nullptr);

  [[nodiscard]] std::string toString() const;
};

#endif //KT2CPP_TOKEN_H