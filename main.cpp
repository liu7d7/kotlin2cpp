#include <iostream>
#include <fstream>
#include <Lexer.h>
#include "Parser.h"
#include "Nodes/VarDeclNode.h"
#include "Nodes/DataclassNode.h"
#include "Transpiler.h"
#include "clipboardxx.hpp"
#include "Nodes/LambdaNode.h"
#include "Nodes/TypealiasNode.h"
#include "Nodes/InterpolatedStringNode.h"

[[nodiscard]] inline std::string getTabs(int nesting) {
    return std::string(nesting * 2, ' ');
}

void print(Node* in, int nesting = 0) {
  std::cout << getTabs(nesting) << NodeType_toString(in->type) << ": ";
  switch (in->type) {
    case N_LIST:
      std::cout << std::endl;
      for (auto& i : ((ListNode*) in)->nodes) {
        print(i, nesting + 1);
      }
      break;
    case N_NUMBER:
      std::cout << ((NumberNode*) in)->value << std::endl;
      break;
    case N_STRING:
      std::cout << ((StringNode*) in)->token->value << std::endl;
      break;
    case N_BIN_OP:
      std::cout << ((BinaryOpNode*) in)->opTok->value << std::endl;
      print(((BinaryOpNode*) in)->left, nesting + 1);
      print(((BinaryOpNode*) in)->right, nesting + 1);
      break;
    case N_UN_OP:
      std::cout << ((UnaryOpNode*) in)->opTok->value << std::endl;
      print(((UnaryOpNode*) in)->node, nesting + 1);
      break;
    case N_VAR_ASSIGN:
      std::cout << ((VarAssignNode*) in)->idTok->value << std::endl;
      print(((VarAssignNode*) in)->value, nesting + 1);
      break;
    case N_VAR_ACCESS:
      if (((VarAccessNode*) in)->idTok)
        std::cout << ((VarAccessNode*) in)->idTok->value << std::endl;
      else std::cout << std::endl;
      break;
    case N_FUNC_DEF:
      std::cout << ((FuncDefNode*) in)->idTok->value << " ";
      for (int i = 0; i < ((FuncDefNode*) in)->args.size(); i++) {
        std::cout << ((FuncDefNode*) in)->args[i]->idTok->value << ": " << ((FuncDefNode*) in)->args[i]->typeNode->toString();
        if (i != ((FuncDefNode*) in)->args.size() - 1) {
          std::cout << ", ";
        }
      }
      std::cout << std::endl;
      print(((FuncDefNode*) in)->body, nesting + 1);
      break;
    case N_VAR_DECL:
      std::cout << ((VarDeclNode*) in)->idTok->value << std::endl;
      if (((VarDeclNode*) in)->value)
        print(((VarDeclNode*) in)->value, nesting + 1);
      break;
    case N_ARG:
      std::cout << ((ArgNode*) in)->idTok->value << ": " << (((ArgNode*) in)->typeNode ? ((ArgNode*) in)->typeNode->toString() : "Any") << std::endl;
      break;
    case N_RETURN:
      std::cout << std::endl;
      print(((ReturnNode*) in)->returnNode, nesting + 1);
      break;
    case N_CALL:
      std::cout << ((CallNode*) in)->toCall->toString() << std::endl;
      for (auto& i : ((CallNode*) in)->args) {
        print(i, nesting + 1);
      }
      break;
    case N_DATACLASS:
      std::cout << ((DataclassNode*) in)->idTok->value << " ";
      for (int i = 0; i < ((DataclassNode*) in)->ctor.size(); i++) {
        std::cout << ((DataclassNode*) in)->ctor[i]->idTok->value << ": " << ((DataclassNode*) in)->ctor[i]->typeNode->toString();
        if (i != ((DataclassNode*) in)->ctor.size() - 1) {
          std::cout << ", ";
        }
      }
      std::cout << std::endl;
      break;
    case N_CLASS: // just like dataclass but with body
      std::cout << ((ClassNode*) in)->idTok->value << " ";
      for (int i = 0; i < ((ClassNode*) in)->ctor.size(); i++) {
        std::cout << ((ClassNode*) in)->ctor[i]->idTok->value << ": " << ((ClassNode*) in)->ctor[i]->typeNode->toString();
        if (i != ((ClassNode*) in)->ctor.size() - 1) {
          std::cout << ", ";
        }
      }
      std::cout << std::endl;
      if (((ClassNode*) in)->body != nullptr) {
        print(((ClassNode*) in)->body, nesting + 1);
      }
      break;
    case N_LAMBDA:
      std::cout << std::endl;
      for (auto& i : ((LambdaNode*) in)->args) {
        print(i, nesting + 1);
      }
      print(((LambdaNode*) in)->body, nesting + 1);
      break;
    case N_IF:
      std::cout << std::endl;
      for (auto& i : ((IfNode*) in)->cases) {
        print(i.first, nesting + 1);
        print(i.second, nesting + 2);
      }
      if (((IfNode*) in)->elseCase != nullptr) {
        std::cout << getTabs(nesting + 1) << "ELSE:" << std::endl;
        print(((IfNode*) in)->elseCase, nesting + 2);
      }
      break;
    case N_PACKAGE:
      for (int i = 0; i < ((PackageNode*) in)->idToks.size(); i++) {
        std::cout << ((PackageNode*) in)->idToks[i]->value;
        if (i != ((PackageNode*) in)->idToks.size() - 1) {
          std::cout << ".";
        }
      }
      std::cout << std::endl;
      if (((PackageNode*) in)->body != nullptr) {
        print(((PackageNode*) in)->body, nesting + 1);
      }
      break;
    case N_WHILE:
    case N_DO_WHILE:
      std::cout << std::endl;
      print(((WhileNode*) in)->condition, nesting + 1);
      print(((WhileNode*) in)->body, nesting + 1);
      break;
    case N_TYPE:
      std::cout << ((TypeNode*) in)->toString() << std::endl;
      break;
    case N_FOR:
      std::cout << ((ForNode*) in)->var->idTok->value << std::endl;
      print(((ForNode*) in)->iterable, nesting + 1);
      print(((ForNode*) in)->body, nesting + 1);
      break;
    case N_BREAK:
      std::cout << std::endl;
      break;
    case N_CONTINUE:
      std::cout << std::endl;
      break;
    case N_IDX:
      std::cout << std::endl;
      print(((IndexNode*) in)->item, nesting + 1);
      print(((IndexNode*) in)->idx, nesting + 1);
      if (((IndexNode*) in)->newVal != nullptr) {
        print(((IndexNode*) in)->newVal, nesting + 1);
      }
      break;
    case N_IMPORT:
      break;
    case N_MAP:
      break;
    case N_TYPEALIAS:
      std::cout << std::endl;
      print(((TypealiasNode*) in)->from, nesting + 1);
      print(((TypealiasNode*) in)->to, nesting + 1);
      break;
    case N_INTERPOLATED_STRING:
      std::cout << std::endl;
      for (auto& i : ((InterpolatedStringNode*) in)->items) {
        print(i, nesting + 1);
      }
      break;
  }
}

int main(int argc, char** argv) {
  const std::string fileName = "test.kt";
  // Get the file contents as a string

  std::ifstream file(fileName);
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string fileContents = buffer.str();

  Lexer lexer(new std::string(fileContents), new std::string(fileName));
  auto tokens = lexer.tokenize();
  if (lexer.error != nullptr) {
    std::cout << lexer.error->toString() << std::endl;
    return 1;
  }
  std::cout << "Begin lexer output:" << std::endl;
  for (auto it : tokens) {
    std::cout << it->toString() << std::endl;
  }
  std::cout << "End lexer output" << std::endl;
  std::cout << std::endl;

  auto ast = Parser(tokens).parse();
  if (ast->error) {
    std::cout << ast->error->toString() << std::endl;
    return 1;
  }
  std::cout << "Begin AST output:" << std::endl;
  print(ast->node);
  std::cout << "End AST output" << std::endl;
  std::cout << std::endl;

  auto res = Transpiler(ast).transpile();
  std::cout << "Begin transpiled output:" << std::endl;
  std::cout << res;
  std::cout << "End transpiled output" << std::endl;

  // save to file
  srand(time(NULL));
  std::string fName = "out" + std::to_string(rand()) + ".cpp";
  std::ofstream out(fName);
  out << res;
  out.close();
  std::cout << "Saved to " << fName << std::endl;
  clipboardxx::clipboard c;
  c << res;
  std::cout << "And copied to clipboard" << std::endl;
  return 0;
}
