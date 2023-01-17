//
// Created by Aadi and Michael on 7/23/22.
//

#ifndef KT2CPP_LEXER_H
#define KT2CPP_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <Token.h>
#include <Error.h>

class Lexer {
private:
  std::string* txt;
  char currentChar;
  Position* pos;
  std::unordered_map<std::string, TokenType> keywords = {
    {"class",     CLASS},
    {"data",      DATA},
    {"in",        IN},
    {"return",    RETURN},
    {"continue",  CONTINUE},
    {"break",     BREAK},
    {"package",   PACKAGE},
    {"fun",       FUN},
    {"as",        AS},
    {"do",        DO},
    {"for",       FOR},
    {"while",     WHILE},
    {"var",       VAR},
    {"val",       VAR},
    {"else",      ELSE},
    {"if",        IF},
    {"xor",       BIT_XOR},
    {"and",       BIT_AND},
    {"or",        BIT_OR},
    {"import",    IMPORT},
    {"step",      STEP},
    {"downTo",    DOWNTO},
    {"until",     UNTIL},
    {"typealias", TYPEALIAS}
  };

  std::unordered_map<char, char> escapeChars = {
    {'n',  '\n'},
    {'t',  '\t'},
    {'r',  '\r'},
    {'b',  '\b'},
    {'f',  '\f'},
    {'a',  '\a'},
    {'\\', '\\'},
    {'"',  '"'},
    {'\'', '\''},
    {'e',  '\033'},
  };
public:
  Error* error = nullptr;

  Lexer(std::string* text, std::string* fileName);

  std::vector<Token*> tokenize();

  void advance();
  void skipComment();

  Token* makeNumber();
  std::vector<Token*> makeString();
  std::vector<Token*> parseInterpolatedString();
  Token* makeIdentifier();
  Token* makeLessThan();
  Token* makeGreaterThan();
  Token* makeQuestionMark();
  Token* makeEquals();
  Token* makeArrow();
  Token* makePlus();
  Token* makeModulo();
  Token* makeMultiply();
  Token* makeDivide();
  Token* makeOr();
  Token* makeAnd();
  Token* makeDot();
  Token* makeNot();
};

#endif //KT2CPP_LEXER_H
