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
#include "Nodes/ForNode.h"
#include "Nodes/IndexNode.h"
#include "Nodes/TypealiasNode.h"
#include "Nodes/LambdaNode.h"
#include "Nodes/InterpolatedStringNode.h"
#include <unordered_set>
#include <iostream>
#include <fstream>

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}

void replaceAllInPlace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
}

std::string Transpiler::translateId(const std::string& in) {
  if (ID_TRANSLATE.find(in) != ID_TRANSLATE.end()) {
    TranslatedIdentifier t = ID_TRANSLATE.at(in);
    if (t.level > translationLevel.top()) return replaceAll(in, "`", "");
    if (t.needsImport) imports.insert(t.name);
    if (t.level == TL_KT_UTIL && KT_UTILS.find(in) != KT_UTILS.end()) {
      ktUtils.insert(in);
    }
    return (t.needsImport ? "std::" : "") + t.name;
  }
  return replaceAll(in, "`", "");
}

std::string getCallNodeMethodName(CallNode* in) {
  if (in->toCall->type == N_VAR_ACCESS) {
    auto* van = (VarAccessNode*) in->toCall;
    return van->methodName->value;
  }
  return "Can't handle";
}

const std::unordered_set<std::string> TOP_LEVEL_MANGLE{
  "println", "print", "Scanner", "Array", "sort", "sortBy"
};

const std::unordered_set<std::string> MEMBER_MANGLE{
  "add", "also", "let", "map", "toString"
};

bool shouldMangleCallNode(CallNode* in) {
  if (in->toCall->type != N_VAR_ACCESS) return false;
  std::string name = getCallNodeMethodName(in);
  if (TOP_LEVEL_MANGLE.find(name) != TOP_LEVEL_MANGLE.end()) return true;
  if (((VarAccessNode*) in->toCall)->members.empty()) return false;
  return MEMBER_MANGLE.find(name) != MEMBER_MANGLE.end();
}

const std::unordered_set<std::string> VAR_ACCESS_MANGLE{
  "System"
};

bool shouldMangleVarAccessToken(Token* in) {
  return VAR_ACCESS_MANGLE.find(in->value) != VAR_ACCESS_MANGLE.end();
}

/**
 *
 * @param in CallNodw in the form of foo.bar()
 * @return CallNode in the form of bar(foo)
 */
CallNode* mangleCallNode(CallNode* in) {
  if (in->toCall->type == N_VAR_ACCESS) {
    auto* van = (VarAccessNode*) in->toCall;
    if (van->idTok && van->members.empty()) { // nothing to mangle, already in from foo(bar)
      return in;
    }
    if (van->parent && van->members.empty()) {
      return in;
    }
    if (van->idTok) { // !van->members.empty() && van->parent == nullptr, mangles from idTok.member1.member2(foo) to member2(idTok.member1, foo) or CallNode(van.methodName, in.generics, {van.idTok, van.members[0..van.members.size() - 1]})
      auto newArgs = std::vector<Node*>();
      auto membersMinusLast = std::vector<Token*>(van->members.begin(), van->members.end() - 1);
      newArgs.push_back(new VarAccessNode(van->idTok, membersMinusLast));
      newArgs.insert(newArgs.end(), in->args.begin(), in->args.end());
      return new CallNode(new VarAccessNode(van->methodName), in->generics, newArgs);
    }
    if (van->parent) {
      auto newArgs = std::vector<Node*>();
      auto membersMinusLast = std::vector<Token*>(van->members.begin(), van->members.end() - 1);
      newArgs.push_back(new VarAccessNode(nullptr, membersMinusLast, van->parent));
      newArgs.insert(newArgs.end(), in->args.begin(), in->args.end());
      return new CallNode(new VarAccessNode(van->methodName), in->generics, newArgs);
    }
  }
  std::cout << "Can't mangle " << NodeType_toString(in->toCall->type) << std::endl;
  exit(1);
}

Transpiler::Transpiler(ParseResult* ast) : ast(ast) {
  translationLevel.push(TL_IDENTIFIERS);
  typeAbove.push(N_LIST);
}

std::string Transpiler::transpile() {
  recurse(ast->node, out);

  std::string mini = buildKtUtils(ktUtils);
  replaceAllInPlace(mini, "\n", "");
  replaceAllInPlace(mini, "\r", "");
  replaceAllInPlace(mini, "  ", "");

  std::stringstream importsOut;
  for (const auto& i : imports) {
    importsOut << "#include <" << i << ">" << '\n';
  }
  importsOut << '\n';

  std::stringstream real;
  real << importsOut.str();
  real << "// kotlin support\n";
  real << mini;
  real << "\n\n";
  real << out.str();
  return real.str();
}

struct LoopSpecification {
  Node* start{};
  Node* end{};
  Node* step{};
  bool inclusive = true;
  bool down = false;
};

LoopSpecification toLoopSpecification(Node* in) {
  if (in->type != N_BIN_OP) {
    std::cout << "Can't convert " << NodeType_toString(in->type) << " to LoopSpecification" << std::endl;
    exit(1);
  }
  auto* binOp = (BinaryOpNode*) in;
  LoopSpecification spec;
  if (binOp->opTok->type == STEP) {
    spec.step = binOp->right;
    binOp = (BinaryOpNode*) binOp->left;
  } else {
    spec.step = new NumberNode(new Token(NUMBER, "1", binOp->posStart, binOp->posEnd));
  }
  switch (binOp->opTok->type) {
    case RANGE:
      spec.start = binOp->left;
      spec.end = binOp->right;
      spec.inclusive = true;
      spec.down = false;
      break;
    case UNTIL:
      spec.start = binOp->left;
      spec.end = binOp->right;
      spec.inclusive = false;
      spec.down = false;
      break;
    case DOWNTO:
      spec.start = binOp->left;
      spec.end = binOp->right;
      spec.inclusive = true;
      spec.down = true;
      break;
    default:
      std::cout << "Expected range, until, or downTo, got " << binOp->opTok->toString() << std::endl;
      exit(1);
  }
  return spec;
}

void Transpiler::recurse(Node* in, std::stringstream& output, int nesting) {
  NodeType above = typeAbove.top();
  if (in->type != N_VAR_ACCESS)
    typeAbove.push(in->type);
  std::stringstream out;
  std::string tabs = std::string(nesting * 2, ' ');
  if (above == N_RETURN || above == N_VAR_ASSIGN || above == N_VAR_DECL) {
    tabs = "";
  }
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
      for (int i = 0; i < package->idToks.size(); i++) {
        out << package->idToks[i]->value;
        if (i != package->idToks.size() - 1) out << "_";
      }
      out << " {" << '\n';
      recurse(package->body, out, nesting + 1);
      out << tabs << "}";
      break;
    }
    case N_CLASS: {
      auto classNode = (ClassNode*) in;
      out << tabs << "struct " << classNode->idTok->value << " {\n";
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
        out << "template<";
        for (int i = 0; i < funcDef->generics.size(); i++) {
          out << "typename " << funcDef->generics[i]->value;
          if (i != funcDef->generics.size() - 1) out << ", ";
        }
        out << ">\n" << tabs;
      }
      if (funcDef->returnType) {
        recurse(funcDef->returnType, out, nesting);
      } else {
        out << (funcDef->idTok->value == "main" ? "int" : "void");
      }
      out << ' ' << funcDef->idTok->value << '(';
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
      out << translateId(type->idTok->value);
      if (!type->generics.empty()) {
        out << "<";
        for (int i = 0; i < type->generics.size(); i++) {
          recurse(type->generics[i], out, 0);
          if (i != type->generics.size() - 1) out << ", ";
        }
        out << ">";
      }
      break;
    }
    case N_ARG: {
      auto arg = (ArgNode*) in;
      bool ref = above == N_FUNC_DEF || above == N_LAMBDA;
      if (arg->typeNode) {
        recurse(arg->typeNode, out, nesting);
        if (ref) {
          out << '&';
        }
      } else {
        out << "auto";
        if (ref) {
          out << '&';
        }
      }
      out << ' ';
      out << arg->idTok->value;
      break;
    }
    case N_LAMBDA: {
      auto lambda = (LambdaNode*) in;
      out << "[&](";
      for (int i = 0; i < lambda->args.size(); i++) {
        recurse(lambda->args[i], out, nesting);
        if (i != lambda->args.size() - 1) out << ", ";
      }
      out << ") {\n";
      recurse(lambda->body, out, nesting + 1);
      out << tabs << "}";
      break;
    }
    case N_DATACLASS: {
      auto dataClass = (DataclassNode*) in;
      imports.insert("string");
      if (!dataClass->generics.empty()) {
        out << tabs << "template<";
        for (int i = 0; i < dataClass->generics.size(); i++) {
          out << "typename " << dataClass->generics[i]->value;
          if (i != dataClass->generics.size() - 1) out << ", ";
        }
        out << ">\n";
      }
      out << tabs << "struct " << dataClass->idTok->value << " {\n";
      for (auto& i : dataClass->ctor) {
        out << tabs << "  ";
        recurse(i, out, nesting + 1);
        out << ';' << '\n';
      }
      out << '\n';
      // default constructer (no args)
      out << tabs << "  " << dataClass->idTok->value << "() {}\n";
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
      out << tabs << "  " << "[[nodiscard]] std::string toString() {\n";
      out << tabs << "    " << "return (std::string(\"" << dataClass->idTok->value << "(\")";
      for (auto& i : dataClass->ctor) {
        out << " + \"" << i->idTok->value << "=\" + __kt__::toString(" << i->idTok->value << ")";
      }
      out << " + \")\");\n";
      out << tabs << "  " << "}" << '\n';
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
        recurse(varDecl->value, out, nesting);
      }
      break;
    }
    case N_BIN_OP: {
      auto binOp = (BinaryOpNode*) in;
      out << ((above == N_CALL || above == N_VAR_ACCESS || above == N_BIN_OP) ? "" : tabs) << "(";
      recurse(binOp->left, out, nesting);
      out << ' ' << binOp->opTok->value << ' ';
      recurse(binOp->right, out, nesting);
      out << ')';
      break;
    }
    case N_VAR_ACCESS: {
      auto varAccess = (VarAccessNode*) in;
      if (varAccess->parent) {
        bool needsParens = !varAccess->members.empty() && varAccess->parent->type != N_CALL;
        if (needsParens)
          out << '(';
        recurse(varAccess->parent, out, nesting);
        if (needsParens)
          out << ')';
        if (!varAccess->members.empty())
          out << '.';
      }
      if (varAccess->idTok) {
        bool mangle = shouldMangleVarAccessToken(varAccess->idTok);
        if (mangle) translationLevel.push(TL_KT_UTIL);
        out << translateId(varAccess->idTok->value) << (varAccess->members.empty() ? "" : ".");
        if (mangle) translationLevel.pop();
      }
      if (!varAccess->members.empty()) {
        for (int i = 0; i < varAccess->members.size(); i++) {
          bool mangle = shouldMangleVarAccessToken(varAccess->members[i]);
          if (mangle) translationLevel.push(TL_KT_UTIL);
          out << translateId(varAccess->members[i]->value);
          if (mangle) translationLevel.pop();
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
      bool mangle = false;
      if (shouldMangleCallNode(call)) {
        mangle = true;
        auto oldCall = call;
        call = mangleCallNode(call);
        if (oldCall != call)
          delete oldCall;
      }
      bool needsParens = call->toCall->type != N_VAR_ACCESS;
      out << ((above == N_CALL || above == N_VAR_ACCESS) ? "" : tabs) << (needsParens ? "(" : "");
      if (mangle) {
        translationLevel.push(TL_KT_UTIL);
      }
      recurse(call->toCall, out, nesting);
      if (mangle) {
        translationLevel.pop();
      }
      out << (needsParens ? ")" : "");
      if (!call->generics.empty()) {
        out << "<";
        for (int i = 0; i < call->generics.size(); i++) {
          recurse(call->generics[i], out, nesting);
          if (i != call->generics.size() - 1) out << ", ";
        }
        out << ">";
      }
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
      recurse(ret->returnNode, out, nesting);
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
      recurse(varAssign->value, out, nesting);
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
    case N_FOR: {
      auto loop = (ForNode*) in;
      out << tabs << "for (";
      recurse(loop->var, out, 0);
      if (loop->iterable->type == N_BIN_OP) { // indexed loop
        auto spec = toLoopSpecification(loop->iterable);
        out << " = ";
        recurse(spec.start, out, 0);
        out << "; ";
        out << loop->var->idTok->value;
        std::string op = spec.inclusive ? (spec.down ? ">=" : "<=") : (spec.down ? ">" : "<");
        out << ' ' << op << ' ';
        recurse(spec.end, out, 0);
        out << "; ";
        out << loop->var->idTok->value;
        out << (spec.down ? " -= " : " += ") << "(";
        recurse(spec.step, out, 0);
        out << ")) {\n";
        recurse(loop->body, out, nesting + 1);
        out << tabs << "}";
      } else {
        out << " : ";
        recurse(loop->iterable, out, 0);
        out << ") {\n";
        recurse(loop->body, out, nesting + 1);
        out << tabs << "}";
      }
      break;
    }
    case N_TYPEALIAS: {
      auto alias = (TypealiasNode*) in;
      if (!alias->from->generics.empty()) {
        out << tabs << "template<";
        for (int i = 0; i < alias->from->generics.size(); i++) {
          out << "typename ";
          recurse(alias->from->generics[i], out, 0);
          if (i != alias->from->generics.size() - 1) out << ", ";
        }
        out << ">\n";
      }
      out << tabs << "using " << alias->from->idTok->value << " = ";
      recurse(alias->to, out, 0);
      break;
    }
    case N_UN_OP:
      break;
    case N_BREAK:
      out << tabs << "break;\n";
      break;
    case N_CONTINUE:
      out << tabs << "continue;\n";
      break;
    case N_IDX: {
      auto idx = (IndexNode*) in;
      if (above != N_CALL)
        out << tabs;
      recurse(idx->item, out, 0);
      out << '[';
      recurse(idx->idx, out, 0);
      out << ']';
      if (idx->newVal) {
        out << " = ";
        recurse(idx->newVal, out, 0);
      }
      break;
    }
    case N_IMPORT: {
      out << tabs << "import;\n";
      break;
    }
    case N_INTERPOLATED_STRING: {
      imports.insert("string");
      auto str = (InterpolatedStringNode*) in;
      if (str->items.empty()) out << "\"\"";
      else {
        out << "(";
        bool first = true;
        for (auto it : str->items) {
          if (!first)
            out << " + ";
          bool needsToString = it->type != N_STRING;
          if (needsToString) {
            ktUtils.insert("toString");
            out << "__kt__::toString(";
          } else if (first) {
            out << "std::string(";
          }
          recurse(it, out, nesting + 1);
          if (needsToString || first) {
            out << ")";
          }
          first = false;
        }
        out << ")";
      }
    }
    case N_MAP:
      break;
  }
  if (in->type != N_VAR_ACCESS)
    typeAbove.pop();
  output << out.str();
}