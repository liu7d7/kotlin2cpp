//
// Created by Aadi and Michael on 7/23/22.
//

#ifndef AT_LEXER_H
#define AT_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <Token.h>
#include <Error.h>

using namespace std;

class Lexer {
private:
  string* txt;
  char currentChar;
  Position* pos;
  unordered_map<string, TokenType> keywords = {
    {"class",    CLASS},
    {"data",     DATA},
    {"in",       IN},
    {"return",   RETURN},
    {"continue", CONTINUE},
    {"break",    BREAK},
    {"package",  PACKAGE},
    {"fun",      FUN},
    {"as",       AS},
    {"do",       DO},
    {"for",      FOR},
    {"while",    WHILE},
    {"var",      VAR},
    {"val",      VAR},
    {"else",     ELSE},
    {"if",       IF},
    {"xor",      BIT_XOR},
    {"and",      BIT_AND},
    {"or",       BIT_OR},
    {"import",   IMPORT},
    {"step",     STEP},
    {"downTo",   DOWNTO}
  };

  unordered_map<char, char> escapeChars = {
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

  Lexer(string* text, string* fileName);

  vector<Token*> tokenize();

  void advance();
  void skipComment();

  Token* makeNumber();
  Token* makeString();
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

#endif //AT_LEXER_H
