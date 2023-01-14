//
// Created by Aadi and Michael on 7/24/22.
//

#ifndef AT_PARSER_H
#define AT_PARSER_H

#include <unordered_set>
#include <unordered_map>
#include <Token.h>
#include <ParseResult.h>
#include <Nodes/UnaryOpNode.h>
#include <Nodes/NumberNode.h>
#include <Nodes/BinaryOpNode.h>
#include <Nodes/VarAssignNode.h>
#include <Nodes/VarAccessNode.h>
#include <Nodes/IfNode.h>
#include <Nodes/ForNode.h>
#include <Nodes/FuncDefNode.h>
#include <Nodes/CallNode.h>
#include <Nodes/WhileNode.h>
#include <Nodes/StringNode.h>
#include <Nodes/ListNode.h>
#include <Nodes/MapNode.h>
#include <Nodes/ReturnNode.h>
#include <Nodes/BreakNode.h>
#include <Nodes/ContinueNode.h>
#include <Nodes/PackageNode.h>
#include <Nodes/ClassNode.h>
#include <Nodes/IndexNode.h>
#include <Nodes/ImportNode.h>

class Parser {
public:
  vector<Token*> tokens;
  Token* currentToken;
  int tokIdx = -1;

  Parser(vector<Token*> tokens);

  Token* advance();
  Token* advanceNewLines();
  Token* reverse(int amt = 1);
  void updateCurrentToken();

  ParseResult* parse();
  ParseResult* statements();
  ParseResult* statement();
  ParseResult* expr();
  ParseResult* binOp(vector<TokenType> ops, ParseResult* (Parser::*funcA)(), ParseResult* (Parser::*funcB)());
  ParseResult* compExpr();
  ParseResult* arithExpr();
  ParseResult* term();
  ParseResult* rangeExpr();
  ParseResult* factor();
  ParseResult* power();
  ParseResult* call();
  ParseResult* atom();
  ParseResult* ifExpr();
  ParseResult* whileExpr();
  ParseResult* doWhileExpr();
  ParseResult* funcDef();
  ParseResult* packageDef();
  ParseResult* classDef();
  ParseResult* dataclassDef();
  ParseResult* importExpr();
  ParseResult* typeExpr();
  ParseResult* checkIndex(Node* n, ParseResult* res);
};

#endif //AT_PARSER_H
