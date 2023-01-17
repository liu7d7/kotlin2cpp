//
// Created by Aadi and Michael on 7/23/22.
//

#include <Lexer.h>
#include <iostream>

Lexer::Lexer(std::string* text, std::string* fileName) {
  this->txt = text;
  currentChar = '\0';
  pos = new Position(-1, 0, -1, fileName, text);
  advance();
}

Token* Lexer::makeNumber() {
  std::string num;
  auto start = pos->copy();
  bool isDecimal = false;
  bool reverse = false;
  Position* firstDecimalPlace;
  int firstDecimalIndex = 0;
  while (isdigit(currentChar) || (currentChar == '.')) {
    num += currentChar;
    if (currentChar == '.') {
      if (isDecimal) { // two decimal places in a number
        reverse = true;
        break;
      }
      firstDecimalPlace = pos->copy();
      firstDecimalIndex = (int) num.length() - 1;
      isDecimal = true;
    }
    advance();
  }
  if (reverse) {
    num = num.substr(0, firstDecimalIndex);
    pos = firstDecimalPlace;
  } else if (currentChar == 'f') {
    num += 'f';
    advance();
  }
  return new Token(NUMBER, num, start, pos->copy());
}

std::vector<Token*> Lexer::makeString() {
  std::string str;
  auto start = pos->copy();
  advance();
  char prevChar = '\0';
  bool escape = false;
  bool isInterpolated = false;
  while (currentChar != '\0' && (escape || currentChar != '"')) {
    if (currentChar == '\\') escape = prevChar != '\\';
    else escape = false;
    if ((isalpha(currentChar) || currentChar == '_' || currentChar == '{') && prevChar == '$') { // this string is an interpolated string, needs special treatment
      isInterpolated = true;
      break;
    }
    str += currentChar;
    prevChar = currentChar;
    advance();
  }
  if (isInterpolated) {
    pos = start;
    advance();
    return parseInterpolatedString();
  }
  advance();
  return {new Token(STRING, str, start, pos->copy())};
}

Token* Lexer::makeIdentifier() {
  std::string id;
  auto start = pos->copy();
  while (isalpha(currentChar) || isdigit(currentChar) || currentChar == '_' || currentChar == '`') {
    id += currentChar;
    advance();
  }
  if (keywords.find(id) != keywords.end()) {
    return new Token(keywords[id], id, start, pos->copy());
  }
  return new Token(IDENTIFIER, id, start, pos->copy());
}

Token* Lexer::makeLessThan() {
  auto start = pos->copy();
  advance();
  if (currentChar == '=') {
    advance();
    return new Token(LESS_THAN_EQUAL, "<=", start, pos->copy());
  }
  return new Token(LESS_THAN, "<", start, pos->copy());
}

Token* Lexer::makeGreaterThan() {
  auto start = pos->copy();
  advance();
  if (currentChar == '=') {
    advance();
    return new Token(GREATER_THAN_EQUAL, ">=", start, pos->copy());
  }
  return new Token(GREATER_THAN, ">", start, pos->copy());
}

Token* Lexer::makeEquals() {
  auto start = pos->copy();
  advance();
  if (currentChar == '=') {
    advance();
    return new Token(EQUAL, "==", start, pos->copy());
  }
  return new Token(ASSIGN, "=", start, pos->copy());
}

Token* Lexer::makeArrow() {
  auto start = pos->copy();
  advance();
  if (currentChar == '>') {
    advance();
    return new Token(ARROW, "->", start, pos->copy());
  }
  if (currentChar == '-') {
    advance();
    return new Token(DECREMENT, "--", start, pos->copy());
  }
  if (currentChar == '=') {
    advance();
    return new Token(MINUS_EQ, "-=", start, pos->copy());
  }
  return new Token(MINUS, "-", start, pos->copy());
}

Token* Lexer::makeDivide() {
  auto start = pos->copy();
  advance();
  if (currentChar == '/') {
    skipComment();
  }
  if (currentChar == '=') {
    advance();
    return new Token(DIVIDE_EQ, "/=", start, pos->copy());
  }
  return new Token(DIVIDE, "/", start, pos->copy());
}

Token* Lexer::makeMultiply() {
  auto start = pos->copy();
  advance();
  if (currentChar == '=') {
    advance();
    return new Token(MULTIPLY_EQ, "*=", start, pos->copy());
  }
  return new Token(MULTIPLY, "*", start, pos->copy());
}

Token* Lexer::makeModulo() {
  auto start = pos->copy();
  advance();
  if (currentChar == '=') {
    advance();
    return new Token(MODULO_EQ, "%=", start, pos->copy());
  }
  return new Token(MODULO, "%", start, pos->copy());
}

Token* Lexer::makePlus() {
  auto start = pos->copy();
  advance();
  if (currentChar == '+') {
    advance();
    return new Token(INCREMENT, "++", start, pos->copy());
  }
  if (currentChar == '=') {
    advance();
    return new Token(PLUS_EQ, "+=", start, pos->copy());
  }
  return new Token(PLUS, "+", start, pos->copy());
}

Token* Lexer::makeQuestionMark() {
  auto start = pos->copy();
  advance();
  if (currentChar == ':') {
    advance();
    return new Token(NUL_TERNARY, "?:", start, pos->copy());
  }
  if (currentChar == '.') {
    advance();
    return new Token(NUL_SAFE, "?.", start, pos->copy());
  }
}

Token* Lexer::makeAnd() {
  auto start = pos->copy();
  advance();
  if (currentChar == '&') {
    advance();
    return new Token(AND, "&&", start, pos->copy());
  }
  // TODO: Handle error
}

Token* Lexer::makeOr() {
  auto start = pos->copy();
  advance();
  if (currentChar == '|') {
    advance();
    return new Token(OR, "", start, pos->copy());
  }
  // TODO: Handle error
}

Token* Lexer::makeNot() {
  auto start = pos->copy();
  advance();
  if (currentChar == '=') {
    advance();
    return new Token(NOT_EQUAL, "!=", start, pos->copy());
  }
  return new Token(NOT, "!", start, pos->copy());
}

Token* Lexer::makeDot() {
  auto start = pos->copy();
  advance();
  if (currentChar == '.') {
    advance();
    return new Token(RANGE, "..", start, pos->copy());
  }
  return new Token(DOT, ".", start, pos->copy());
}

std::vector<Token*> Lexer::tokenize() {
  std::vector<Token*> tokens;
  while (currentChar != '\0') {
    if (currentChar == ' ' || currentChar == '\t') {
      advance();
    } else if (currentChar == '\n' || currentChar == ';') {
      tokens.emplace_back(new Token(NEWLINE, "\\n", pos));
      advance();
    } else if (currentChar == '+') {
      tokens.push_back(makePlus());
    } else if (currentChar == '-') {
      tokens.push_back(makeArrow());
    } else if (currentChar == '*') {
      tokens.push_back(makeMultiply());
    } else if (currentChar == '/') {
      tokens.push_back(makeDivide());
    } else if (currentChar == '{') {
      tokens.emplace_back(new Token(LCURLYBRACE, "{", pos));
      advance();
    } else if (currentChar == '}') {
      tokens.emplace_back(new Token(RCURLYBRACE, "}", pos));
      advance();
    } else if (currentChar == '(') {
      tokens.emplace_back(new Token(LPAREN, "(", pos));
      advance();
    } else if (currentChar == ')') {
      tokens.emplace_back(new Token(RPAREN, ")", pos));
      advance();
    } else if (currentChar == ':') {
      tokens.emplace_back(new Token(COLON, ":", pos));
      advance();
    } else if (currentChar == '?') {
      tokens.push_back(makeQuestionMark());
    } else if (currentChar == ',') {
      tokens.emplace_back(new Token(COMMA, ",", pos));
      advance();
    } else if (currentChar == '%') {
      tokens.push_back(makeModulo());
    } else if (currentChar == '&') {
      tokens.push_back(makeAnd());
    } else if (currentChar == '|') {
      tokens.push_back(makeOr());
    } else if (currentChar == '[') {
      tokens.emplace_back(new Token(LBRACKET, "[", pos));
      advance();
    } else if (currentChar == ']') {
      tokens.emplace_back(new Token(RBRACKET, "]", pos));
      advance();
    } else if (currentChar == '.') {
      tokens.push_back(makeDot());
    } else if (currentChar == '=') {
      tokens.push_back(makeEquals());
    } else if (currentChar == '!') {
      tokens.push_back(makeNot());
    } else if (currentChar == '<') {
      tokens.emplace_back(makeLessThan());
    } else if (currentChar == '>') {
      tokens.emplace_back(makeGreaterThan());
    } else if (isdigit(currentChar)) {
      tokens.emplace_back(makeNumber());
    } else if (currentChar == '"') {
      std::vector<Token*> thing = makeString();
      tokens.insert(tokens.cend(), thing.begin(), thing.end());
    } else if (isalpha(currentChar) || currentChar == '_' || currentChar == '`') {
      tokens.emplace_back(makeIdentifier());
    } else {
      error = new Error(pos, pos, "Unexpected character", std::to_string(currentChar));
      return {};
    }
  }
  tokens.emplace_back(new Token(END_OF_FILE, "", pos));
  return tokens;
}

void Lexer::advance() {
  pos->advance(currentChar);
  if (pos->idx >= txt->length()) {
    currentChar = '\0';
  } else {
    currentChar = txt->at(pos->idx);
  }
}

void Lexer::skipComment() {
  while (currentChar != '\n' && currentChar != '\0') {
    advance();
  }
}

std::vector<Token*> Lexer::parseInterpolatedString() {
  auto start = pos->copy();
  std::vector<Token*> toks;
  toks.push_back(new Token(IS_BEGIN, "\"", start, pos->copy()));
  std::stringstream contents;
  bool escape = false;
  char prevChar = '\0';
  while (currentChar != '\0' && currentChar != '"') {
    if (currentChar == '\\') escape = prevChar != '\\';
    else escape = false;
    if (currentChar == '$') {
      advance();
      if (isalpha(currentChar) || currentChar == '_') {
        toks.push_back(new Token(STRING, contents.str(), start, pos->copy()));
        contents.str("");
        toks.push_back(new Token(IS_EXPR_BEGIN, "$", pos->copy(), pos->copy()));
        toks.push_back(makeIdentifier());
        toks.push_back(new Token(IS_EXPR_END, "", pos->copy(), pos->copy()));
      } else if (currentChar == '{') {
        int curlyBraceCount = 1;
        toks.push_back(new Token(STRING, contents.str(), start, pos->copy()));
        contents.str("");
        advance();
        auto exprStart = pos->copy();
        std::stringstream toLex;
        while (currentChar != '\0' && curlyBraceCount > 0) {
          if (currentChar == '{') {
            curlyBraceCount++;
          } else if (currentChar == '}') {
            curlyBraceCount--;
          }
          if (curlyBraceCount == 0) {
            advance();
            break;
          }
          toLex << currentChar;
          advance();
        }
        std::vector<Token*> lexed = Lexer(new std::string(toLex.str()), pos->fName).tokenize();
        if (lexed.empty()) {
          error = Lexer(new std::string(toLex.str()), pos->fName).error;
          return {};
        }
        lexed.pop_back();
        if (!lexed.empty()) {
          toks.push_back(new Token(IS_EXPR_BEGIN, "${", exprStart, pos->copy()));
          toks.insert(toks.cend(), lexed.begin(), lexed.end());
          toks.push_back(new Token(IS_EXPR_END, "}", pos->copy(), pos->copy()));
        }
      } else {
        contents << '$' << currentChar;
        advance();
      }
    } else {
      contents << currentChar;
      advance();
      prevChar = currentChar;
    }
  }
  toks.push_back(new Token(STRING, contents.str(), start, pos->copy()));
  advance();
  toks.push_back(new Token(IS_END, "\"", start, pos->copy()));
  // remove all empty text tokens
  auto it = toks.begin();
  while (it != toks.cend()) {
    if ((*it)->type == STRING && (*it)->value.empty()) {
      it = toks.erase(it);
    } else {
      it++;
    }
  }
  return toks;
}
