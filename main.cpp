#include <iostream>
#include <fstream>
#include <Lexer.h>
#include "Parser.h"
#include "Nodes/VarDeclNode.h"
#include "Nodes/DataclassNode.h"
#include "Transpiler.h"

[[nodiscard]] inline string getTabs(int nesting) {
    return string(nesting * 2, ' ');
}

void print(Node* in, int nesting = 0) {
  std::cout << getTabs(nesting) << NodeType_toString(in->type) << ": ";
  switch (in->type) {
    case N_LIST:
      cout << endl;
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
      std::cout << ((VarAccessNode*) in)->idTok->value << std::endl;
      break;
    case N_FUNC_DEF:
      std::cout << ((FuncDefNode*) in)->nameTok->value << " ";
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
      print(((VarDeclNode*) in)->value, nesting + 1);
      break;
    case N_ARG:
      std::cout << ((ArgNode*) in)->idTok->value << ": " << ((ArgNode*) in)->typeNode->toString() << std::endl;
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
      std::cout << ((ClassNode*) in)->nameTok->value << " ";
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
      for (int i = 0; i < ((PackageNode*) in)->nameToks.size(); i++) {
        std::cout << ((PackageNode*) in)->nameToks[i]->value;
        if (i != ((PackageNode*) in)->nameToks.size() - 1) {
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
  }
}

int main(int argc, char** argv) {
  const string fileName = "test.kt";
  // Get the file contents as a string

  ifstream file(fileName);
  stringstream buffer;
  buffer << file.rdbuf();
  string fileContents = buffer.str();

  Lexer lexer(new string(fileContents), new string(fileName));
  auto tokens = lexer.tokenize();
  if (lexer.error != nullptr) {
    cout << lexer.error->toString() << endl;
    return 1;
  }
  cout << "Begin lexer output:" << endl;
  for (auto it : tokens) {
    cout << it->toString() << endl;
  }
  cout << "End lexer output" << endl;
  cout << endl;

  auto ast = Parser(tokens).parse();
  if (ast->error) {
    cout << ast->error->toString() << endl;
    return 1;
  }
  cout << "Begin AST output:" << endl;
  print(ast->node);
  cout << "End AST output" << endl;
  cout << endl;

  auto res = Transpiler(ast).transpile();
  cout << "Begin transpiled output:" << endl;
  cout << res;
  cout << "End transpiled output" << endl;
  return 0;
}
