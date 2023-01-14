//
// Created by richard may clarkson on 14/01/2023.
//
#include "Transpiler.h"
#include "Nodes/ListNode.h"
#include "Nodes/PackageNode.h"
#include "Nodes/ClassNode.h"
#include "Nodes/FuncDefNode.h"
#include "Nodes/DataclassNode.h"
#include "Nodes/VarDeclNode.h"
#include "Nodes/BinaryOpNode.h"
#include "Nodes/VarAccessNode.h"
#include "Nodes/WhileNode.h"
#include "Nodes/CallNode.h"
#include "Nodes/StringNode.h"
#include "Nodes/NumberNode.h"
#include "Nodes/ReturnNode.h"
#include "Nodes/IfNode.h"
#include "Nodes/VarAssignNode.h"
#include <unordered_set>

const unordered_set<NodeType> NO_SEMICOLONS {
  NodeType::N_IF, NodeType::N_WHILE, NodeType::N_FOR, NodeType::N_FUNC_DEF, NodeType::N_PACKAGE
};

const unordered_set<NodeType> DOUBLE_NEWLINE {
  NodeType::N_FUNC_DEF, NodeType::N_PACKAGE, NodeType::N_CLASS, NodeType::N_DATACLASS, NodeType::N_IF, NodeType::N_WHILE, NodeType::N_FOR, NodeType::N_DO_WHILE
};

Transpiler::Transpiler(ParseResult* ast) : ast(ast) {

}

string Transpiler::transpile() {
  recurse(ast->node, out);

  return out.str();
}

struct LoopSpecification {
  string start;
  string end;
  string step;
  bool inclusive;
};

LoopSpecification toLoopSpecification(Node* in) {

}

void Transpiler::recurse(Node* in, stringstream& output, int nesting) {
  stringstream out;
  string tabs = string(nesting * 2, ' ');
  switch (in->type) {
    case N_LIST: {
      auto list = (ListNode*) in;
      for (int i = 0; i < list->nodes.size(); i++) {
        auto n = list->nodes[i];
        recurse(n, out, nesting);
        if (NO_SEMICOLONS.find(n->type) == NO_SEMICOLONS.end()) {
          out << ";";
        }
        if (DOUBLE_NEWLINE.find(n->type) != DOUBLE_NEWLINE.end() && i != list->nodes.size() - 1) {
          out << '\n';
        }
        out << '\n';
      }
      break;
    }
    case N_PACKAGE: {
      auto package = (PackageNode*) in;
      out << tabs << "namespace ";
      for (int i = 0; i < package->nameToks.size(); i++) {
        out << package->nameToks[i]->value;
        if (i != package->nameToks.size() - 1) out << "_";
      }
      out << " {" << '\n';
      recurse(package->body, out, nesting + 1);
      out << tabs << "}";
      break;
    }
    case N_CLASS: {
      auto classNode = (ClassNode*) in;
      out << tabs << "struct " << classNode->nameTok->value << " {\n";
      if (classNode->body) {
        recurse(classNode->body, out, nesting + 1);
      } else {
        out << '\n';
      }
      out << tabs << "}";
      break;
    }
    case N_FUNC_DEF: {
      auto funcDef = (FuncDefNode*) in;
      out << tabs;
      if (!funcDef->generics.empty()) {
        out << "template <";
        for (int i = 0; i < funcDef->generics.size(); i++) {
          out << "typename " << funcDef->generics[i]->value;
          if (i != funcDef->generics.size() - 1) out << ", ";
        }
        out << ">\n" << tabs;
      }
      if (funcDef->returnType) {
        recurse(funcDef->returnType, out, nesting);
      } else {
        out << "void";
      }
      out << ' ' << funcDef->nameTok->value << '(';
      for (int i = 0; i < funcDef->args.size(); i++) {
        recurse(funcDef->args[i], out, nesting);
        if (i != funcDef->args.size() - 1) out << ", ";
      }
      out << ") {" << '\n';
      recurse(funcDef->body, out, nesting + 1);
      out << tabs << "}";
      break;
    }
    case N_TYPE: {
      auto type = (TypeNode*) in;
      out << type->typeTok->value;
      if (!type->generics.empty()) {
        out << "<";
        for (int i = 0; i < type->generics.size(); i++) {
          out << type->generics[i]->value;
          if (i != type->generics.size() - 1) out << ", ";
        }
        out << ">";
      }
      break;
    }
    case N_ARG: {
      auto arg = (ArgNode*) in;
      recurse(arg->typeNode, out, nesting);
      out << ' ';
      out << arg->idTok->value;
      break;
    }
    case N_DATACLASS: {
      auto dataClass = (DataclassNode*) in;
      if (!dataClass->generics.empty()) {
        out << tabs << "template <";
        for (int i = 0; i < dataClass->generics.size(); i++) {
          out << "typename " << dataClass->generics[i]->value;
          if (i != dataClass->generics.size() - 1) out << ", ";
        }
        out << ">\n";
      }
      out << tabs << "struct " << dataClass->idTok->value << " {\n";
      for (auto & i : dataClass->ctor) {
        out << tabs << "  ";
        recurse(i, out, nesting + 1);
        out << ';' << '\n';
      }
      out << '\n';
      out << tabs << "  " << dataClass->idTok->value << '(';
      for (int i = 0; i < dataClass->ctor.size(); i++) {
        recurse(dataClass->ctor[i], out, nesting + 1);
        if (i != dataClass->ctor.size() - 1) out << ", ";
      }
      out << ") : ";
      for (int i = 0; i < dataClass->ctor.size(); i++) {
        out << dataClass->ctor[i]->idTok->value << '(' << dataClass->ctor[i]->idTok->value << ')';
        if (i != dataClass->ctor.size() - 1) out << ", ";
      }
      out << " {}" << '\n';
      out << tabs << "}";
      break;
    }
    case N_VAR_DECL: {
      auto varDecl = (VarDeclNode*) in;
      out << tabs;
      if (varDecl->typeNode) {
        recurse(varDecl->typeNode, out, nesting);
      } else {
        out << "auto";
      }
      out << ' ' << varDecl->idTok->value;
      if (varDecl->value) {
        out << " = ";
        recurse(varDecl->value, out, 0);
      }
      break;
    }
    case N_BIN_OP: {
      auto binOp = (BinaryOpNode*) in;
      out << '(';
      recurse(binOp->left, out, nesting);
      out << ' ' << binOp->opTok->value << ' ';
      recurse(binOp->right, out, nesting);
      out << ')';
      break;
    }
    case N_VAR_ACCESS: {
      auto varAccess = (VarAccessNode*) in;
      if (varAccess->parent) {
        out << '(';
        recurse(varAccess->parent, out, nesting);
        out << ')';
        out << '.';
      }
      if (varAccess->idTok) {
        out << varAccess->idTok->value << (varAccess->members.empty() ? "" : ".");
      }
      if (!varAccess->members.empty()) {
        for (int i = 0; i < varAccess->members.size(); i++) {
          out << varAccess->members[i]->value;
          if (i != varAccess->members.size() - 1) out << '.';
        }
      }
      break;
    }
    case N_WHILE: {
      auto whileNode = (WhileNode*) in;
      out << tabs << "while (";
      recurse(whileNode->condition, out, 0);
      out << ") {\n";
      recurse(whileNode->body, out, nesting + 1);
      out << tabs << "}";
      break;
    }
    case N_DO_WHILE: {
      auto whileNode = (WhileNode*) in;
      out << tabs << "do {\n";
      recurse(whileNode->body, out, nesting + 1);
      out << tabs << "} while (";
      recurse(whileNode->condition, out, 0);
      out << ")";
      break;
    }
    case N_CALL: {
      auto call = (CallNode*) in;
      bool needsParens = call->toCall->type != N_VAR_ACCESS;
      out << tabs << (needsParens ? "(" : "");
      recurse(call->toCall, out, nesting);
      out << (needsParens ? ")" : "");
      out << '(';
      for (int i = 0; i < call->args.size(); i++) {
        recurse(call->args[i], out, nesting);
        if (i != call->args.size() - 1) out << ", ";
      }
      out << ')';
      break;
    }
    case N_STRING: {
      auto string = (StringNode*) in;
      out << '"' << string->token->value << '"';
      break;
    }
    case N_NUMBER: {
      auto number = (NumberNode*) in;
      out << number->token->value;
      break;
    }
    case N_RETURN: {
      auto ret = (ReturnNode*) in;
      out << tabs << "return ";
      recurse(ret->returnNode, out, 0);
      break;
    }
    case N_VAR_ASSIGN: {
      auto varAssign = (VarAssignNode*) in;
      out << tabs;
      if (varAssign->idTok) {
        out << varAssign->idTok->value;
        if (!varAssign->members.empty()) {
          out << '.';
        }
      }
      if (!varAssign->members.empty()) {
        for (int i = 0; i < varAssign->members.size(); i++) {
          out << varAssign->members[i]->value;
          if (i != varAssign->members.size() - 1) out << '.';
        }
      }
      out << " = ";
      recurse(varAssign->value, out, 0);
      break;
    }
    case N_IF: {
      auto ifNode = (IfNode*) in;
      out << tabs << "if (";
      recurse(ifNode->cases[0].first, out, 0);
      out << ") {\n";
      recurse(ifNode->cases[0].second, out, nesting + 1);
      out << tabs << "}";
      for (int i = 1; i < ifNode->cases.size(); i++) {
        out << " else if (";
        recurse(ifNode->cases[i].first, out, 0);
        out << ") {\n";
        recurse(ifNode->cases[i].second, out, nesting + 1);
        out << tabs << "}";
      }
      if (ifNode->elseCase) {
        out << " else {\n";
        recurse(ifNode->elseCase, out, nesting + 1);
        out << tabs << "}";
      }
      break;
    }
  }
  output << out.str();
}
