//
// Created by Aadi and Michael on 7/25/22.
//

#include <Parser.h>
#include <utility>
#include <iostream>
#include "Nodes/VarDeclNode.h"
#include "Nodes/ArgNode.h"
#include "Nodes/DataclassNode.h"
#include "Nodes/TypealiasNode.h"
#include "Nodes/LambdaNode.h"
#include "Nodes/InterpolatedStringNode.h"
#include "Nodes/EmptyNode.h"
#include "Nodes/CharNode.h"

const std::string INVALID_SYNTAX = "InvalidSyntaxError";

Parser::Parser(std::vector<Token*> tokens) {
  this->tokens = std::move(tokens);
  this->currentToken = nullptr;
  tokIdx = -1;
  advance();
}

Token* Parser::advance() {
  tokIdx++;
  updateCurrentToken();
  return currentToken;
}

Token* Parser::advanceNewLines() {
  while (currentToken->type == NEWLINE) {
    advance();
  }
  return currentToken;
}

Token* Parser::reverse(int amt) {
  tokIdx -= amt;
  updateCurrentToken();
  return currentToken;
}

void Parser::updateCurrentToken() {
  if (tokIdx >= 0 and tokIdx < tokens.size()) {
    currentToken = tokens[tokIdx];
  }
}

#define CALLSTACK_PUSH callStack.emplace(__FUNCTION__)

ParseResult* Parser::parse() {
  CALLSTACK_PUSH;
  auto res = statements();
  if (!res->error && currentToken->type != END_OF_FILE) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                  "Unknown cause at token with type " + TokenType_toString(currentToken->type) +
                                  " top of callstack: " + callStack.top()));
  }


  return res;
}

ParseResult* Parser::statements() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  std::vector<Node*> statements;
  auto posStart = currentToken->posStart->copy();

  while (currentToken->type == NEWLINE) {
    res->regAdvancement();
    advance();
  }

  Node* stmt = res->reg(statement());
  if (res->error) {

    return res;
  }
  statements.push_back(stmt);

  bool moreStatements = true;

  while (true) {
    int newLineCount = 0;
    while (currentToken->type == NEWLINE) {
      res->regAdvancement();
      advance();
      newLineCount++;
    }
    if (newLineCount == 0) {
      moreStatements = false;
    }
    if (!moreStatements) break;
    auto st = statement();
    if (st->error && st->error->message != "STATEMENTS_STOP") {
      return res->failure(st->error);
    }
    stmt = res->tryReg(st);
    if (!stmt) {
      reverse(res->reverseCount);
      moreStatements = false;
      continue;
    }
    statements.push_back(stmt);
  }

  return res->success(new ListNode(statements, posStart, currentToken->posEnd->copy()));
}

ParseResult* Parser::statement() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  auto posStart = currentToken->posStart->copy();

  if (currentToken->type == RETURN) {
    delete currentToken;
    res->regAdvancement();
    advance();
    Node* exp = res->tryReg(expr());
    if (!exp) reverse(res->reverseCount);


    return res->success(new ReturnNode(exp, posStart, currentToken->posEnd->copy()));
  } else if (currentToken->type == CONTINUE) {
    delete currentToken;
    res->regAdvancement();
    advance();


    return res->success(new ContinueNode(posStart, currentToken->posEnd->copy()));
  } else if (currentToken->type == BREAK) {
    delete currentToken;
    res->regAdvancement();
    advance();


    return res->success(new BreakNode(posStart, currentToken->posEnd->copy()));
  }

  Node* exp = res->reg(expr());
  if (res->error) {

    return res;
  }

  return res->success(exp);
}

ParseResult* Parser::call() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  Node* ato = res->reg(atom());
  if (res->error) {

    return res;
  }

  Node* supplied = res->reg(suppliedCall(ato));
  if (res->error) {

    return res;
  }


  return res->success(supplied);
}

ParseResult* Parser::atom() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  auto tok = currentToken;
  Node* n = nullptr;

  if (tok->type == NUMBER) {
    res->regAdvancement();
    advance();
    n = new NumberNode(tok);
  } else if (tok->type == STRING) {
    res->regAdvancement();
    advance();
    n = new StringNode(tok);
  } else if (tok->type == T_CHAR) {
    res->regAdvancement();
    advance();
    n = new CharNode(tok);
  } else if (tok->type == IS_BEGIN) {
    auto is = res->reg(interpolatedStringExpr());
    if (res->error) {

      return res;
    }
    n = is;
  } else if (tok->type == LPAREN) {
    res->regAdvancement();
    advance();
    Node* exp = res->reg(expr());
    if (res->error) {

      return res;
    }

    if (currentToken->type == RPAREN) {
      res->regAdvancement();
      advance();
      n = exp;
    } else {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
    }
  } else if (tok->type == LCURLYBRACE) { // lambda expr
    Node* exp = res->reg(lambdaExpr());
    if (res->error) {

      return res;
    }
    n = exp;
  } else if (tok->type == VAR) {
    // delete useless (from now on) var keyword token
    delete currentToken;
    res->regAdvancement();
    advance();
    // if not identifier, return error
    if (currentToken->type != IDENTIFIER) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    // save current token as identifier
    auto idTok = currentToken;
    res->regAdvancement();
    advance();

    // handle type
    if (currentToken->type == COLON) {
      // delete useless (from now on) colon token
      delete currentToken;
      res->regAdvancement();
      advance();

      auto typeTok = (TypeNode*) res->reg(typeExpr());
      if (currentToken->type == ASSIGN) {
        // delete useless (from now on) assign token
        delete currentToken;
        res->regAdvancement();
        advance();

        Node* exp = res->reg(expr());
        if (res->error) {

          return res;
        }

        n = new VarDeclNode(idTok, typeTok, exp);
      } else {
        n = new VarDeclNode(idTok, typeTok, nullptr);
      }
    } else if (currentToken->type == ASSIGN) { // handle w/o type
      // delete useless (from now on) assign token
      delete currentToken;
      res->regAdvancement();
      advance();

      Node* exp = res->reg(expr());
      if (res->error) {

        return res;
      }

      n = new VarDeclNode(idTok, nullptr /* no type info available */, exp);
    } else {
      n = new VarDeclNode(idTok, nullptr /* no type info available */, nullptr);
    }
  } else if (tok->type == IDENTIFIER) {
    Node* exp = res->reg(idExpr());
    if (res->error) {

      return res;
    }
    n = exp;
  } else if (tok->type == IF) {
    Node* exp = res->reg(ifExpr());
    if (res->error) {

      return res;
    }
    n = exp;
  } else if (tok->type == FUN) {
    Node* funDef = res->reg(funcDef());
    if (res->error) {

      return res;
    }
    n = funDef;
  } else if (tok->type == PACKAGE) {
    Node* package = res->reg(packageDef());
    if (res->error) {

      return res;
    }
    n = package;
  } else if (tok->type == CLASS) {
    Node* type = res->reg(classDef());
    if (res->error) {

      return res;
    }
    n = type;
  } else if (tok->type == DATA) {
    Node* dat = res->reg(dataclassDef());
    if (res->error) {

      return res;
    }
    n = dat;
  } else if (tok->type == DO) {
    Node* doWhile = res->reg(doWhileExpr());
    if (res->error) {

      return res;
    }
    n = doWhile;
  } else if (tok->type == WHILE) {
    Node* loop = res->reg(whileExpr());
    if (res->error) {

      return res;
    }
    n = loop;
  } else if (tok->type == TYPEALIAS) {
    Node* typealias = res->reg(typealiasDef());
    if (res->error) {

      return res;
    }
    n = typealias;
  } else if (tok->type == FOR) {
    Node* loop = res->reg(forExpr());
    if (res->error) {

      return res;
    }
    n = loop;
  } else if (tok->type == IMPORT) {
    // TODO: Make statement? I dunno since I think imports are more statement than expression
    Node* import = res->reg(importExpr());
    if (res->error) {

      return res;
    }
    n = import;
  }

  if (n == nullptr) {
    if (currentToken->type == RCURLYBRACE) {
      return res->failure(new Error(tok->posStart, tok->posEnd, INVALID_SYNTAX, "STATEMENTS_STOP"));
    }
    return res->failure(new Error(tok->posStart, tok->posEnd, INVALID_SYNTAX,
                                  "Expected 'number', identifier, '+', '-', '@', '?', '&' or '(', got " +
                                  TokenType_toString(tok->type)));
  }
  checkIndex(n, res);

  return res;
}

ParseResult* Parser::factor() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  auto tok = currentToken;

  if (tok->type == PLUS || tok->type == MINUS) {
    res->regAdvancement();
    advance();
    Node* _factor = res->reg(factor());
    if (res->error) {

      return res;
    }

    return res->success(new UnaryOpNode(tok, _factor));
  }
  auto ret = power();

  return ret;
}

ParseResult* Parser::power() {
  CALLSTACK_PUSH;
  // Unimplemented, just here so order of operations is readable
  auto ret = binOp({}, &Parser::call, &Parser::factor);;

  return ret;
}

ParseResult* Parser::term() {
  CALLSTACK_PUSH;
  auto ret = binOp({MULTIPLY, DIVIDE, MODULO}, &Parser::factor, &Parser::factor);

  return ret;
}

ParseResult* Parser::arithExpr() {
  CALLSTACK_PUSH;
  auto ret = binOp({PLUS, MINUS}, &Parser::term, &Parser::term);

  return ret;
}

ParseResult* Parser::rangeExpr() {
  CALLSTACK_PUSH;
  auto ret = binOp({RANGE, DOWNTO, STEP, UNTIL}, &Parser::arithExpr, &Parser::arithExpr);

  return ret;
}

ParseResult* Parser::compExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();

  if (currentToken->type == NOT) {
    auto opTok = currentToken;
    res->regAdvancement();
    advance();

    Node* exp = res->reg(compExpr());
    if (res->error) return res;
    return res->success(new UnaryOpNode(opTok, exp));
  }

  Node* exp = res->reg(
    binOp({EQUAL, NOT_EQUAL, LESS_THAN, LESS_THAN_EQUAL, GREATER_THAN, GREATER_THAN_EQUAL}, &Parser::rangeExpr,
          &Parser::rangeExpr));
  if (res->error) {

    return res;
  }


  return res->success(exp);
}

ParseResult* Parser::assignExpr() {
  CALLSTACK_PUSH;
  auto ret = binOp({PLUS_EQ, MINUS_EQ}, &Parser::compExpr, &Parser::compExpr);

  return ret;
}

ParseResult* Parser::expr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();

  Node* n = res->reg(binOp({AND, OR}, &Parser::assignExpr, &Parser::assignExpr));
  if (res->error) {

    return res;
  }

  if (currentToken->type == DOT) {
    delete currentToken;
    res->regAdvancement();
    advance();

    std::vector<Token*> memberNames;
    if (currentToken->type != IDENTIFIER) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    memberNames.emplace_back(currentToken);
    res->regAdvancement();
    advance();

    while (currentToken->type == DOT) {
      delete currentToken;
      res->regAdvancement();
      advance();

      if (currentToken->type != IDENTIFIER) {

        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
      }
      memberNames.emplace_back(currentToken);
      res->regAdvancement();
      advance();
    }


    return res->success(new VarAccessNode(nullptr, memberNames, n));
  }


  return res->success(n);
}

ParseResult* Parser::importExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  while (currentToken->type != NEWLINE) {
    delete currentToken;
    res->regAdvancement();
    advance();
  }


  return res->success(new EmptyNode());
}

ParseResult* Parser::ifExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  std::vector<std::pair<Node*, Node*>> cases;
  Node* elseCase = nullptr;

  if (currentToken->type != IF) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected if"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type != LPAREN) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '('"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* cond = res->reg(expr());
  if (res->error) {

    return res;
  }

  if (currentToken->type != RPAREN) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type == LCURLYBRACE) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* body = res->reg(statements());
    if (res->error) {

      return res;
    }

    if (currentToken->type == RCURLYBRACE) {
      delete currentToken;
      res->regAdvancement();
      advance();

      cases.emplace_back(cond, body);
    } else {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
    }
  } else {
    Node* exp = res->reg(statement());
    if (res->error) {

      return res;
    }
    cases.emplace_back(cond, exp);
  }

  bool hasElse = false;
  while (currentToken->type == ELSE) {
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IF) {
      hasElse = true;
      break;
    }

    delete currentToken;
    res->regAdvancement();
    advance();

    cond = res->reg(expr());
    if (res->error) {

      return res;
    }

    if (currentToken->type == LCURLYBRACE) {
      delete currentToken;
      res->regAdvancement();
      advance();

      Node* body = res->reg(statements());
      if (res->error) {

        return res;
      }

      if (currentToken->type == RCURLYBRACE) {
        delete currentToken;
        res->regAdvancement();
        advance();
        cases.emplace_back(cond, body);
      } else {

        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
      }
    } else {
      Node* exp = res->reg(statement());
      if (res->error) {

        return res;
      }
      cases.emplace_back(cond, exp);
    }
  }

  if (hasElse) {
    if (currentToken->type == LCURLYBRACE) {
      delete currentToken;
      res->regAdvancement();
      advance();

      elseCase = res->reg(statements());
      if (res->error) {

        return res;
      }

      if (currentToken->type == RCURLYBRACE) {
        delete currentToken;
        res->regAdvancement();
        advance();
      } else {

        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
      }
    } else {
      elseCase = res->reg(statement());
      if (res->error) {

        return res;
      }
    }
  }


  return res->success(new IfNode(cases, elseCase));
}

ParseResult* Parser::funcDef() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();

  if (currentToken->type != FUN) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'fun'"));
  }
  // delete now useless function token
  delete currentToken;
  res->regAdvancement();
  advance();

  Token* idTok = nullptr;
  std::vector<Token*> generics;
  if (currentToken->type == LESS_THAN) { // handle generics
    delete currentToken;
    res->regAdvancement();
    advance();

    while (currentToken->type == IDENTIFIER) {
      generics.push_back(currentToken);
      res->regAdvancement();
      advance();

      if (currentToken->type == COMMA) {
        delete currentToken;
        res->regAdvancement();
        advance();
      } else if (currentToken->type == GREATER_THAN) {
        delete currentToken;
        res->regAdvancement();
        advance();
        break;
      } else {

        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ',' or '>'"));
      }
    }
  }
  // get identifier
  if (currentToken->type == IDENTIFIER) {
    idTok = currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != LPAREN) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '('"));
    }
  } else { // anonymous function
    if (currentToken->type != LPAREN) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier or '('"));
    }
  }

  // delete useless lparen token
  delete currentToken;
  res->regAdvancement();
  advance();

  std::vector<ArgNode*> args;
  if (currentToken->type == RPAREN) goto ArgsEnd;
  if (currentToken->type != IDENTIFIER) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  while (currentToken->type == IDENTIFIER) {
    auto* arg = (ArgNode*) res->reg(argExpr(true, false));
    args.push_back(arg);
    if (currentToken->type == RPAREN) break;
    if (currentToken->type != COMMA) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ','"));
    }
    // delete useless comma token
    delete currentToken;
    res->regAdvancement();
    advance();
  }

  ArgsEnd:;

  if (currentToken->type != RPAREN) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
  }

  // delete useless rparen token
  delete currentToken;
  res->regAdvancement();
  advance();

  TypeNode* returnType = nullptr;
  if (currentToken->type == COLON) {
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IDENTIFIER) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    returnType = (TypeNode*) res->reg(typeExpr());
  }

  if (currentToken->type == ASSIGN) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* body = res->reg(statement());
    if (res->error) {

      return res;
    }

    return res->success(new FuncDefNode(idTok, returnType, generics, args, body, true));
  } else if (currentToken->type == LCURLYBRACE) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* body = res->reg(statements());
    if (res->error) {

      return res;
    }

    if (currentToken->type == RCURLYBRACE) {
      delete currentToken;
      res->regAdvancement();
      advance();

      return res->success(new FuncDefNode(idTok, returnType, generics, args, body, false));
    } else {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
    }
  } else {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '->' or '{'"));
  }
}

ParseResult* Parser::packageDef() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  if (currentToken->type != PACKAGE) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'package'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type != IDENTIFIER) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }

  std::vector<Token*> idToks;
  while (currentToken->type == IDENTIFIER) {
    Token* idTok = currentToken;
    res->regAdvancement();
    advance();
    idToks.push_back(idTok);
    if (currentToken->type == NEWLINE) {
      break;
    }
    if (currentToken->type == DOT) {
      delete currentToken;
      res->regAdvancement();
      advance();
    } else {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '.'"));
    }
  }

  Node* body = res->reg(statements());
  if (res->error) {

    return res;
  }


  return res->success(new PackageNode(idToks, body));
}

ParseResult* Parser::classDef() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  if (currentToken->type != CLASS) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'class'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();
  if (currentToken->type != IDENTIFIER) {

    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  Token* idTok = currentToken;
  res->regAdvancement();
  advance();

  std::vector<Token*> generics;
  if (currentToken->type == LESS_THAN) {
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IDENTIFIER) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    while (currentToken->type == IDENTIFIER) {
      Token* generic = currentToken;
      res->regAdvancement();
      advance();
      generics.push_back(generic);
      if (currentToken->type == GREATER_THAN) break;
      if (currentToken->type != COMMA) {

        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ','"));
      }
      delete currentToken;
      res->regAdvancement();
      advance();
    }
    if (currentToken->type != GREATER_THAN) {

      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '>'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
  }

  // TODO: inheritance

  std::vector<ArgNode*> ctor;
  if (currentToken->type == LPAREN) { // constructor
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != RPAREN) {
      delete currentToken;
      res->regAdvancement();
      advance();
      if (currentToken->type != IDENTIFIER) {
        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
      }
      while (currentToken->type == IDENTIFIER) {
        auto* arg = (ArgNode*) res->reg(argExpr(true, false));
        ctor.push_back(arg);
        if (currentToken->type == RPAREN) break;
        if (currentToken->type != COMMA) {
          return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ','"));
        }
        // delete useless comma token
        delete currentToken;
        res->regAdvancement();
        advance();
      }
    }
    if (currentToken->type != RPAREN) {
      return res->failure(
        new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
    }
  }

  if (currentToken->type != LCURLYBRACE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '{'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  while (currentToken->type == NEWLINE) {
    res->regAdvancement();
    advance();
  }

  Node* body = nullptr;
  if (currentToken->type != RCURLYBRACE) {
    body = res->reg(statements());
    if (res->error) return res;
  }

  if (currentToken->type != RCURLYBRACE) {
    return res->failure(
      new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  return res->success(new ClassNode(idTok, generics, ctor, body));
}

ParseResult* Parser::checkIndex(Node* n, ParseResult* res) {
  CALLSTACK_PUSH;
  if (currentToken->type == LBRACKET) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* idx = res->reg(expr());
    if (res->error) return res;
    if (currentToken->type == RBRACKET) {
      delete currentToken;
      res->regAdvancement();
      advance();

      if (currentToken->type == ASSIGN) {
        delete currentToken;
        res->regAdvancement();
        advance();

        Node* val = res->reg(expr());
        if (res->error) return res;

        return res->success(new IndexNode(n, idx, val));
      }
      return res->success(new IndexNode(n, idx));
    } else {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ']'"));
    }
  }
  return res->success(n);
}

ParseResult*
Parser::binOp(std::unordered_set<TokenType> ops, ParseResult* (Parser::*funcA)(), ParseResult* (Parser::*funcB)()) {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  auto left = res->reg((this->*funcA)());
  if (res->error) return res;

  // check if ops contains currentToken->type
  while (ops.find(currentToken->type) != ops.end()) {
    auto op = currentToken;
    res->regAdvancement();
    advance();
    Node* right = res->reg((this->*funcB)());
    if (res->error) return res;
    left = new BinaryOpNode(op, left, right);
  }

  return res->success(left);
}

ParseResult* Parser::dataclassDef() {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type != DATA) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'data'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();
  if (currentToken->type != CLASS) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'class'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();
  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  Token* idTok = currentToken;
  res->regAdvancement();
  advance();

  std::vector<Token*> generics;
  if (currentToken->type == LESS_THAN) {
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IDENTIFIER) {
      return res->failure(
        new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    while (currentToken->type == IDENTIFIER) {
      Token* generic = currentToken;
      res->regAdvancement();
      advance();
      generics.push_back(generic);
      if (currentToken->type == GREATER_THAN) break;
      if (currentToken->type != COMMA) {
        return res->failure(
          new Error(currentToken, INVALID_SYNTAX, "Expected ','"));
      }
      delete currentToken;
      res->regAdvancement();
      advance();
    }
    if (currentToken->type != GREATER_THAN) {
      return res->failure(
        new Error(currentToken, INVALID_SYNTAX, "Expected '>'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
  }

  if (currentToken->type != LPAREN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '('"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();
  if (currentToken->type != VAR) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  std::vector<ArgNode*> args;
  while (currentToken->type == IDENTIFIER) {
    auto* arg = (ArgNode*) res->reg(argExpr(true, false));
    args.push_back(arg);
    if (currentToken->type == RPAREN) break;
    if (currentToken->type != COMMA) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ','"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type == VAR) {
      delete currentToken;
      res->regAdvancement();
      advance();
    }
  }

  if (currentToken->type != RPAREN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();
  return res->success(new DataclassNode(idTok, generics, args));
}

ParseResult* Parser::whileExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult();
  if (currentToken->type != WHILE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'while'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* condition = res->reg(expr());
  if (res->error) return res;

  Node* body = nullptr;
  if (currentToken->type == LCURLYBRACE) {
    delete currentToken;
    res->regAdvancement();
    advance();

    while (currentToken->type == NEWLINE) {
      res->regAdvancement();
      advance();
    }

    if (currentToken->type != RCURLYBRACE) {
      body = res->reg(statements());
      if (res->error) return res;
    }

    if (currentToken->type != RCURLYBRACE) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
  } else {
    if (currentToken->type == NEWLINE) {
      delete currentToken;
      res->regAdvancement();
      advance();
    }
    body = res->reg(statement());
    if (res->error) return res;
  }

  return res->success(new WhileNode(condition, body, false));
}

ParseResult* Parser::doWhileExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type != DO) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'do'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* body = nullptr;
  if (currentToken->type == LCURLYBRACE) {
    delete currentToken;
    res->regAdvancement();
    advance();

    body = res->reg(statements());
    if (res->error) return res;

    if (currentToken->type != RCURLYBRACE) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();

  } else {
    if (currentToken->type == NEWLINE) {
      delete currentToken;
      res->regAdvancement();
      advance();
    }
    std::cout << TokenType_toString(currentToken->type) << std::endl;

    body = res->reg(statement());
    if (res->error) {
      std::cout << TokenType_toString(currentToken->type) << std::endl;
      return res;
    }
    if (currentToken->type == NEWLINE) {
      delete currentToken;
      res->regAdvancement();
      advance();
    }
  }

  std::cout << TokenType_toString(currentToken->type) << std::endl;

  if (currentToken->type != WHILE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'while'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  std::cout << TokenType_toString(currentToken->type) << std::endl;

  if (currentToken->type != LPAREN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '('"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  std::cout << TokenType_toString(currentToken->type) << std::endl;

  Node* condition = res->reg(expr());
  if (res->error) return res;

  std::cout << TokenType_toString(currentToken->type) << std::endl;

  if (currentToken->type != RPAREN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  return res->success(new WhileNode(condition, body, true));
}

ParseResult* Parser::typeExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }

  Token* idTok = currentToken;
  res->regAdvancement();
  advance();

  std::vector<TypeNode*> generics;
  if (currentToken->type == LESS_THAN) {
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IDENTIFIER) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    auto gen = (TypeNode*) res->reg(typeExpr());
    if (res->error) return res;
    generics.push_back(gen);
    while (currentToken->type == COMMA) {
      delete currentToken;
      res->regAdvancement();
      advance();
      if (currentToken->type != IDENTIFIER) {
        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
      }
      gen = (TypeNode*) res->reg(typeExpr());
      if (res->error) return res;
      generics.push_back(gen);
    }
    if (currentToken->type != GREATER_THAN) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '>'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
  }

  return res->success(new TypeNode(idTok, generics));
}

ParseResult* Parser::forExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type != FOR) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'for'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type != LPAREN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '('"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  auto* var = (ArgNode*) res->reg(argExpr(false, true));

  if (currentToken->type != IN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'in'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* iter = res->reg(expr());
  if (res->error) return res;

  if (currentToken->type != RPAREN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* body = nullptr;
  if (currentToken->type == LCURLYBRACE) {
    delete currentToken;
    res->regAdvancement();
    advance();

    body = res->reg(statements());
    if (res->error) return res;

    if (currentToken->type != RCURLYBRACE) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
  } else {
    body = res->reg(statement());
    if (res->error) return res;
  }

  return res->success(new ForNode(var, iter, body));
}

ParseResult* Parser::typealiasDef() {
  CALLSTACK_PUSH;
  auto* res = new ParseResult;
  if (currentToken->type != TYPEALIAS) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'typealias'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }

  auto* from = (TypeNode*) res->reg(typeExpr());
  if (res->error) return res;

  if (currentToken->type != ASSIGN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '='"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  auto* to = (TypeNode*) res->reg(typeExpr());
  if (res->error) return res;

  return res->success(new TypealiasNode(from, to));
}

ParseResult* Parser::argExpr(bool allowDefaultArgument, bool optionalType) {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  auto argNode = new ArgNode;
  argNode->idTok = currentToken;
  res->regAdvancement();
  advance();
  if (currentToken->type != COLON) {
    if (optionalType) goto Tail;
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ':'"));
  }
  // delete useless colon token
  delete currentToken;
  res->regAdvancement();
  advance();
  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  argNode->typeNode = (TypeNode*) res->reg(typeExpr());
  if (currentToken->type == ASSIGN) { // default arg
    delete currentToken;
    res->regAdvancement();
    advance();
    Node* defaultValue = res->reg(expr());
    if (res->error) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected expression"));
    }
    argNode->defaultValue = defaultValue;
  }
  Tail:;
  return res->success(argNode);
}

ParseResult* Parser::lambdaExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type != LCURLYBRACE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '{'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  std::vector<ArgNode*> argToks;
  bool hasArgs = false;
  {
    if (currentToken->type == LPAREN) {
      hasArgs = true;
      advance();
      goto Tail;
    }
    advance();
    if (currentToken->type == COMMA || currentToken->type == ARROW || currentToken->type == COLON) hasArgs = true;
    reverse();
    Tail:;
  }
  if (hasArgs) {
    while (currentToken->type == IDENTIFIER) {
      auto arg = (ArgNode*) res->reg(argExpr(false, true));
      if (res->error) return res;
      argToks.push_back(arg);
      if (currentToken->type == COMMA) {
        advance();
      } else if (currentToken->type == ARROW) {
        break;
      } else {
        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ',' or '->'"));
      }
    }
  } else {
    argToks = {
      new ArgNode(new Token(IDENTIFIER, "it", currentToken->posStart, currentToken->posEnd), nullptr, nullptr)};
  }
  if (hasArgs && currentToken->type != ARROW) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '->'"));
  }
  if (hasArgs) {
    delete currentToken;
    res->regAdvancement();
    advance();
  }

  Node* body = res->reg(statements());
  if (res->error) return res;
  const std::unordered_set<NodeType> EXCLUDE_RET{N_RETURN, N_BREAK, N_CONTINUE, N_FOR, N_WHILE, N_DO_WHILE};
  if (body->type == N_LIST) {
    auto list = (ListNode*) body;
    if (!list->nodes.empty() && EXCLUDE_RET.find(list->nodes.back()->type) == EXCLUDE_RET.end()) {
      list->nodes.back() = new ReturnNode(list->nodes.back(), list->nodes.back()->posStart, list->nodes.back()->posEnd);
    }
  }

  if (currentToken->type != RCURLYBRACE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  return res->success(new LambdaNode(body, argToks));
}

ParseResult* Parser::idExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  auto id = currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type == ASSIGN) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* exp = res->reg(expr());
    if (res->error) return res;

    return res->success(new VarAssignNode(id, exp));
  } else if (currentToken->type == DOT) {
    delete currentToken;
    res->regAdvancement();
    advance();

    std::vector<Token*> memberNames;
    if (currentToken->type != IDENTIFIER) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                    "Expected identifier"));
    }
    memberNames.emplace_back(currentToken);
    res->regAdvancement();
    advance();

    while (currentToken->type == DOT) {
      delete currentToken;
      res->regAdvancement();
      advance();

      if (currentToken->type != IDENTIFIER) {
        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
      }
      memberNames.emplace_back(currentToken);
      res->regAdvancement();
      advance();
    }

    if (currentToken->type == ASSIGN) {
      delete currentToken;
      res->regAdvancement();
      advance();

      Node* exp = res->reg(expr());
      if (res->error) return res;

      return res->success(new VarAssignNode(id, exp, memberNames));
    } else {
      return res->success(new VarAccessNode(id, memberNames));
    }
  } else {
    return res->success(new VarAccessNode(id));
  }
}

ParseResult* Parser::suppliedCall(Node* ato) {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type == DOT) {
    delete currentToken;
    res->regAdvancement();
    advance();

    std::vector<Token*> memberNames;
    if (currentToken->type != IDENTIFIER) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                    "Expected identifier"));
    }
    memberNames.emplace_back(currentToken);
    res->regAdvancement();
    advance();

    while (currentToken->type == DOT) {
      delete currentToken;
      res->regAdvancement();
      advance();

      if (currentToken->type != IDENTIFIER) {
        return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                      "Expected identifier"));
      }
      memberNames.emplace_back(currentToken);
      res->regAdvancement();
      advance();
    }

    ato = new VarAccessNode(nullptr, memberNames, ato);
  }
  std::vector<TypeNode*> generics;
  if (currentToken->type == LESS_THAN) {
    delete currentToken;
    res->regAdvancement();
    advance();

    auto t = (TypeNode*) res->reg(typeExpr());
    if (res->error) return res;
    generics.push_back(t);

    while (currentToken->type == COMMA) {
      delete currentToken;
      res->regAdvancement();
      advance();

      t = (TypeNode*) res->reg(typeExpr());
      if (res->error) return res;
      generics.push_back(t);
    }

    if (currentToken->type != GREATER_THAN) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                    "Expected '>'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
  }
  bool isFunc = false;
  std::vector<Node*> args;
  if (currentToken->type == LCURLYBRACE) { // just a lambda, etc .also { ... }
    isFunc = true;
    auto lambda = (LambdaNode*) res->reg(lambdaExpr());
    if (res->error) return res;
    args.push_back(lambda);
  } else if (currentToken->type == LPAREN) {
    isFunc = true;
    delete currentToken;
    res->regAdvancement();
    advance();

    if (currentToken->type == RPAREN) {
      delete currentToken;
      res->regAdvancement();
      advance();
    } else {
      args.emplace_back(res->reg(expr()));
      if (res->error)
        return res;

      while (currentToken->type == COMMA) {
        delete currentToken;
        res->regAdvancement();
        advance();

        args.emplace_back(res->reg(expr()));
        if (res->error) return res;
      }

      if (currentToken->type != RPAREN) {
        return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                      "Expected ')' or ','"));
      }
      delete currentToken;
      res->regAdvancement();
      advance();

      if (currentToken->type == LCURLYBRACE) { // args + a lambda, etc .also { ... }
        isFunc = true;
        auto lambda = (LambdaNode*) res->reg(lambdaExpr());
        if (res->error) return res;
        args.push_back(lambda);
      }
    }
  }

  if (isFunc) {
    ato = new CallNode(ato, generics, args);
  }

  checkIndex(ato, res);

  if (currentToken->type == DOT) {
    auto call = (CallNode*) res->reg(suppliedCall(ato));
    if (res->error) return res;
    return res->success(call);
  }

  return res;
}

ParseResult* Parser::interpolatedStringExpr() {
  CALLSTACK_PUSH;
  auto res = new ParseResult;
  if (currentToken->type != IS_BEGIN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "INTERNAL: Expected 'IS_BEGIN'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  std::vector<Node*> parts;
  while (currentToken->type != IS_END) {
    switch (currentToken->type) {
      case STRING:
        parts.emplace_back(new StringNode(currentToken));
        res->regAdvancement();
        advance();
        break;
      case IS_EXPR_BEGIN:
        delete currentToken;
        res->regAdvancement();
        advance();

        parts.emplace_back(res->reg(statement()));
        if (res->error) return res;

        if (currentToken->type != IS_EXPR_END) {
          return res->failure(new Error(currentToken, INVALID_SYNTAX, "INTERNAL: Expected 'IS_EXPR_END'"));
        }
        delete currentToken;
        res->regAdvancement();
        advance();
        break;
      default:
        return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                      "INTERNAL: Expected 'IS_END', 'std::string', 'IS_BEGIN', or 'IDENTIFIER'"));
    }
  }

  delete currentToken;
  res->regAdvancement();
  advance();

  return res->success(new InterpolatedStringNode(parts));
}
