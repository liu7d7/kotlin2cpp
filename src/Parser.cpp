//
// Created by Aadi and Michael on 7/25/22.
//

#include <Parser.h>

#include <utility>
#include <iostream>
#include "Nodes/VarDeclNode.h"
#include "Nodes/ArgNode.h"
#include "Nodes/DataclassNode.h"

const string INVALID_SYNTAX = "InvalidSyntaxError";

Parser::Parser(vector<Token*> tokens) {
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

ParseResult* Parser::parse() {
  auto res = statements();
  if (!res->error && currentToken->type != END_OF_FILE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                  "Unknown cause at token with type " + TokenType_toString(currentToken->type)));
  }

  return res;
}

ParseResult* Parser::statements() {
  auto res = new ParseResult();
  vector<Node*> statements;
  auto posStart = currentToken->posStart->copy();

  while (currentToken->type == NEWLINE) {
    res->regAdvancement();
    advance();
  }

  Node* stmt = res->reg(statement());
  if (res->error) return res;
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
    stmt = res->tryReg(statement());
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
  if (res->error)
//    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'number', identifier, '?', '+', '-', '&', '@', '[', '(', 'while' or 'not'"));
    return res;
  return res->success(exp);
}

ParseResult* Parser::call() {
  auto res = new ParseResult();
  Node* ato = res->reg(atom());
  if (res->error) return res;

  if (currentToken->type == DOT) {
    delete currentToken;
    res->regAdvancement();
    advance();

    vector<Token*> memberNames;
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
  if (currentToken->type == LPAREN) {
    delete currentToken;
    res->regAdvancement();
    advance();
    vector<Node*> args;

    if (currentToken->type == RPAREN) {
      delete currentToken;
      res->regAdvancement();
      advance();
    } else {
      args.emplace_back(res->reg(expr()));
      if (res->error)
//        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')', '?', '@', '+', '-', '[', number or identifier"));
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
    }

    ato = new CallNode(ato, args);
  }

  checkIndex(ato, res);

  return res;
}

ParseResult* Parser::atom() {
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
  } else if (tok->type == LPAREN) {
    res->regAdvancement();
    advance();
    Node* exp = res->reg(expr());
    if (res->error) return res;

    if (currentToken->type == RPAREN) {
      res->regAdvancement();
      advance();
      n = exp;
    } else {
      return res->failure(
        new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
    }
  } else if (tok->type == VAR) {
    // delete useless (from now on) var keyword token
    delete currentToken;
    res->regAdvancement();
    advance();
    // if not identifier, return error
    if (currentToken->type != IDENTIFIER) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                    "Expected identifier"));
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

      auto typeTok = (TypeNode*)res->reg(typeExpr());
      if (currentToken->type != ASSIGN) {
        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '='"));
      }
      // delete useless (from now on) assign token
      delete currentToken;
      res->regAdvancement();
      advance();

      Node* exp = res->reg(expr());
      if (res->error) return res;

      n = new VarDeclNode(idTok, typeTok, exp);
    } else if (currentToken->type == ASSIGN) { // handle w/o type
      // delete useless (from now on) assign token
      delete currentToken;
      res->regAdvancement();
      advance();

      Node* exp = res->reg(expr());
      if (res->error) return res;

      n = new VarDeclNode(idTok, nullptr /* no type info available */, exp);
    } else {
      return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                    "Expected '=' or ':'"));
    }
  } else if (tok->type == IDENTIFIER) {
    auto id = currentToken;
    res->regAdvancement();
    advance();

    if (currentToken->type == ASSIGN) {
      delete currentToken;
      res->regAdvancement();
      advance();

      Node* exp = res->reg(expr());
      if (res->error) return res;

      n = new VarAssignNode(id, exp);
    } else if (currentToken->type == DOT) {
      delete currentToken;
      res->regAdvancement();
      advance();

      vector<Token*> memberNames;
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

        n = new VarAssignNode(id, exp, memberNames);
      } else {
        n = new VarAccessNode(id, memberNames);
      }
    } else {
      n = new VarAccessNode(id);
    }
  } else if (tok->type == IF) {
    Node* exp = res->reg(ifExpr());
    if (res->error) return res;
    n = exp;
  } else if (tok->type == FUN) {
    Node* funDef = res->reg(funcDef());
    if (res->error) return res;
    n = funDef;
  } else if (tok->type == PACKAGE) {
    Node* package = res->reg(packageDef());
    if (res->error) return res;
    n = package;
  } else if (tok->type == CLASS) {
    Node* type = res->reg(classDef());
    if (res->error) return res;
    n = type;
  } else if (tok->type == DATA) {
    Node* dat = res->reg(dataclassDef());
    if (res->error) return res;
    n = dat;
  } else if (tok->type == DO) {
    Node* doWhile = res->reg(doWhileExpr());
    if (res->error) return res;
    n = doWhile;
  } else if (tok->type == WHILE) {
    Node* loop = res->reg(whileExpr());
    if (res->error) return res;
    n = loop;
  } else if (tok->type == IMPORT) {
    // TODO: Make statement? I dunno since I think imports are more statement than expression
    Node* import = res->reg(importExpr());
    if (res->error) return res;
    n = import;
  }

  if (n == nullptr) {
    return res->failure(new Error(tok->posStart, tok->posEnd, INVALID_SYNTAX, "Expected 'number', identifier, '+', '-', '@', '?', '&' or '(', got " + TokenType_toString(tok->type)));
  }
  checkIndex(n, res);
  return res;
}

ParseResult* Parser::factor() {
  auto res = new ParseResult();
  auto tok = currentToken;

  if (tok->type == PLUS || tok->type == MINUS) {
    res->regAdvancement();
    advance();
    Node* _factor = res->reg(factor());
    if (res->error) return res;
    return res->success(new UnaryOpNode(tok, _factor));
  }
  return power();
}

ParseResult* Parser::power() {
  // Unimplemented, just here so order of operations is readable
  return binOp(vector<TokenType>{}, &Parser::call, &Parser::factor);
}

ParseResult* Parser::term() {
  return binOp({MULTIPLY, DIVIDE, MODULO}, &Parser::factor, &Parser::factor);
}

ParseResult* Parser::arithExpr() {
  return binOp({PLUS, MINUS}, &Parser::term, &Parser::term);
}
ParseResult* Parser::rangeExpr() {
  return binOp({RANGE, DOWNTO, STEP, UNTIL}, &Parser::arithExpr, &Parser::arithExpr);
}

ParseResult* Parser::compExpr() {
  auto res = new ParseResult();

  if (currentToken->type == NOT) {
    auto opTok = currentToken;
    res->regAdvancement();
    advance();

    Node* exp = res->reg(compExpr());
    if (res->error) return res;
    return res->success(new UnaryOpNode(opTok, exp));
  }

  Node* exp = res->reg(binOp({EQUAL, NOT_EQUAL, LESS_THAN, LESS_THAN_EQUAL, GREATER_THAN, GREATER_THAN_EQUAL}, &Parser::rangeExpr, &Parser::rangeExpr));
  if (res->error)
//    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'number', identifier, '+', '-', '&', '@', '[', '(' or 'not'"));
    return res;

  return res->success(exp);
}

ParseResult* Parser::expr() {
  auto res = new ParseResult();

  Node* n = res->reg(binOp({AND, OR}, &Parser::compExpr, &Parser::compExpr));
  if (res->error)
//    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'number', identifier, '+', '-', '?', '@', '[', '&' or '('"));
    return res;

  if (currentToken->type == DOT) {
    delete currentToken;
    res->regAdvancement();
    advance();

    vector<Token*> memberNames;
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

    return res->success(new VarAccessNode(nullptr, memberNames, n));
  }

  return res->success(n);
}

ParseResult* Parser::importExpr() {
  auto res = new ParseResult();
  if (currentToken->type != IMPORT) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                  "Expected '~'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type == PLUS) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* path = res->reg(expr());
    if (res->error) return res;

    return res->success(new ImportNode(path));
  }

  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                  "Expected an identifier or '+'"));
  }
  Token* module = currentToken;
  res->regAdvancement();
  advance();

  return res->success(new ImportNode(module));
}

ParseResult* Parser::ifExpr() {
  auto res = new ParseResult();
  vector<pair<Node*, Node*>> cases;
  Node* elseCase = nullptr;

  if (currentToken->type != IF) {
    return res->failure(
      new Error(currentToken, INVALID_SYNTAX, "Expected if"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* cond = res->reg(expr());
  if (res->error) return res;

  if (currentToken->type == LCURLYBRACE) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* body = res->reg(statements());
    if (res->error) return res;

    if (currentToken->type == RCURLYBRACE) {
      delete currentToken;
      res->regAdvancement();
      advance();

      cases.emplace_back(make_pair(cond, body));
    } else {
      return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                    "Expected '}'"));
    }
  } else {
    Node* exp = res->reg(statement());
    if (res->error) return res;
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
    if (res->error) return res;

    if (currentToken->type == LCURLYBRACE) {
      delete currentToken;
      res->regAdvancement();
      advance();

      Node* body = res->reg(statements());
      if (res->error) return res;

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
      if (res->error) return res;
      cases.emplace_back(cond, exp);
    }
  }

  if (hasElse) {
    if (currentToken->type == LCURLYBRACE) {
      delete currentToken;
      res->regAdvancement();
      advance();

      elseCase = res->reg(statements());
      if (res->error) return res;

      if (currentToken->type == RCURLYBRACE) {
        delete currentToken;
        res->regAdvancement();
        advance();
      } else {
        return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                      "Expected '}'"));
      }
    } else {
      elseCase = res->reg(statement());
      if (res->error) return res;
    }
  }

  return res->success(new IfNode(cases, elseCase));
}

ParseResult* Parser::funcDef() {
  auto res = new ParseResult();

  if (currentToken->type != FUN) {
    return res->failure(
      new Error(currentToken, INVALID_SYNTAX, "Expected 'fun'"));
  }
  // delete now useless function token
  delete currentToken;
  res->regAdvancement();
  advance();

  Token* idTok = nullptr;
  vector<Token*> generics;
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

  vector<ArgNode*> args;
  if (currentToken->type == RPAREN) goto ArgsEnd;
  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  while (currentToken->type == IDENTIFIER) {
    auto argNode = new ArgNode;
    argNode->idTok = currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != COLON) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ':'"));
    }
    // delete useless colon token
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IDENTIFIER) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    argNode->typeNode = (TypeNode*)res->reg(typeExpr());
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
    args.push_back(argNode);
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
    returnType = (TypeNode*)res->reg(typeExpr());
  }

  if (currentToken->type == ASSIGN) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* body = res->reg(statement());
    if (res->error) return res;
    return res->success(new FuncDefNode(idTok, returnType, generics, args, body, true));
  } else if (currentToken->type == LCURLYBRACE) {
    delete currentToken;
    res->regAdvancement();
    advance();

    Node* body = res->reg(statements());
    if (res->error) return res;

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

  vector<Token*> idToks;
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
  if (res->error) return res;

  return res->success(new PackageNode(idToks, body));
}

ParseResult* Parser::classDef() {
  auto res = new ParseResult();
  if (currentToken->type != CLASS) {
    return res->failure(
      new Error(currentToken, INVALID_SYNTAX, "Expected 'class'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();
  if (currentToken->type != IDENTIFIER) {
    return res->failure(
      new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }
  Token* idTok = currentToken;
  res->regAdvancement();
  advance();

  vector<Token*> generics;
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

  // TODO: inheritance

  vector<ArgNode*> ctor;
  if (currentToken->type == LPAREN) { // constructor
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != RPAREN) {
      delete currentToken;
      res->regAdvancement();
      advance();
      if (currentToken->type != IDENTIFIER) {
        return res->failure(
          new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
      }
      while (currentToken->type == IDENTIFIER) {
        auto argNode = new ArgNode;
        argNode->idTok = currentToken;
        res->regAdvancement();
        advance();
        if (currentToken->type != COLON) {
          return res->failure(
            new Error(currentToken, INVALID_SYNTAX, "Expected ':'"));
        }
        // delete useless colon token
        delete currentToken;
        res->regAdvancement();
        advance();
        if (currentToken->type != IDENTIFIER) {
          return res->failure(
            new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
        }
        argNode->typeNode = (TypeNode*)res->reg(typeExpr());
        if (currentToken->type == ASSIGN) { // default arg
          delete currentToken;
          res->regAdvancement();
          advance();
          Node* defaultValue = res->reg(expr());
          if (res->error) {
            return res->failure(
              new Error(currentToken, INVALID_SYNTAX, "Expected expression"));
          }
          argNode->defaultValue = defaultValue;
        }
        ctor.push_back(argNode);
        if (currentToken->type == RPAREN) break;
        if (currentToken->type != COMMA) {
          return res->failure(
            new Error(currentToken, INVALID_SYNTAX, "Expected ','"));
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

      if (currentToken->type == COLON) {
        delete currentToken;
        res->regAdvancement();
        advance();

        Node* val = res->reg(expr());
        if (res->error) return res;

        return res->success(new IndexNode(n, idx, val));
      }
      return res->success(new IndexNode(n, idx));
    } else {
      return res->failure(new Error(currentToken, INVALID_SYNTAX,
                                    "Expected ']'"));
    }
  }
  return res->success(n);
}

ParseResult* Parser::binOp(vector<TokenType> ops, ParseResult* (Parser::*funcA)(), ParseResult* (Parser::*funcB)()) {
  auto res = new ParseResult();
  auto left = res->reg((this->*funcA)());
  if (res->error) return res;

  // check if ops contains currentToken->type
  while (std::find(ops.begin(), ops.end(), currentToken->type) != ops.end()) {
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

  vector<Token*> generics;
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
  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }

  vector<ArgNode*> args;
  while (currentToken->type == IDENTIFIER) {
    auto arg = new ArgNode;
    arg->idTok = currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != COLON) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ':'"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IDENTIFIER) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    arg->typeNode = (TypeNode*)res->reg(typeExpr());
    if (currentToken->type == ASSIGN) { // default arg
      delete currentToken;
      res->regAdvancement();
      advance();
      Node* defaultArg = res->reg(expr());
      arg->defaultValue = defaultArg;
    }
    args.push_back(arg);
    if (currentToken->type == RPAREN) break;
    if (currentToken->type != COMMA) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ','"));
    }
    delete currentToken;
    res->regAdvancement();
    advance();
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
  auto res = new ParseResult();
  if (currentToken->type != WHILE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'while'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* condition = res->reg(expr());
  if (res->error) return res;

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
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  return res->success(new WhileNode(condition, body, false));
}

ParseResult* Parser::doWhileExpr() {
  auto res = new ParseResult;
  if (currentToken->type != DO) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'do'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type != LCURLYBRACE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '{'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  Node* body = res->reg(statements());
  if (res->error) return res;

  if (currentToken->type != RCURLYBRACE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected '}'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  if (currentToken->type != WHILE) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected 'while'"));
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

  Node* condition = res->reg(expr());
  if (res->error) return res;

  if (currentToken->type != RPAREN) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected ')'"));
  }
  delete currentToken;
  res->regAdvancement();
  advance();

  return res->success(new WhileNode(condition, body, true));
}

ParseResult* Parser::typeExpr() {
  auto res = new ParseResult;
  if (currentToken->type != IDENTIFIER) {
    return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
  }

  Token* idTok = currentToken;
  res->regAdvancement();
  advance();

  vector<Token*> generics;
  if (currentToken->type == LESS_THAN) {
    delete currentToken;
    res->regAdvancement();
    advance();
    if (currentToken->type != IDENTIFIER) {
      return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
    }
    generics.push_back(currentToken);
    res->regAdvancement();
    advance();
    while (currentToken->type == COMMA) {
      delete currentToken;
      res->regAdvancement();
      advance();
      if (currentToken->type != IDENTIFIER) {
        return res->failure(new Error(currentToken, INVALID_SYNTAX, "Expected identifier"));
      }
      generics.push_back(currentToken);
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

  return res->success(new TypeNode(idTok, generics));
}
