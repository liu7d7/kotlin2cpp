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
#include <unordered_set>
#include <iostream>
#include <fstream>

const unordered_set<NodeType> NO_SEMICOLONS{
  N_IF, N_WHILE, N_FOR, N_FUNC_DEF, N_PACKAGE
};

const unordered_set<NodeType> DOUBLE_NEWLINE{
  N_FUNC_DEF, N_PACKAGE, N_CLASS, N_DATACLASS, N_IF, N_WHILE, N_FOR, N_DO_WHILE, N_TYPEALIAS
};

struct TranslatedIdentifier {
  string name;
  TranslateLevel level;
  bool needsImport = false;
};

struct KtUtil {
  string contents;
  unordered_set<string> imports{};
};

const unordered_map<string, TranslatedIdentifier> ID_TRANSLATE{
  {"HashMap",     {"unordered_map",         TL_IDENTIFIERS, true}},
  {"HashSet",     {"unordered_set",         TL_IDENTIFIERS, true}},
  {"ArrayList",   {"vector",                TL_IDENTIFIERS, true}},
  {"MutableList", {"vector",                TL_IDENTIFIERS, true}},
  {"LinkedList",  {"list",                  TL_IDENTIFIERS, true}},
  {"Pair",        {"utility",               TL_IDENTIFIERS, true}},
  {"String",      {"string",                TL_IDENTIFIERS, true}},
  {"System",      {"__kt__::System",        TL_KT_UTIL}},
  {"Scanner",     {"__kt__::Scanner",       TL_KT_UTIL}},
  {"println",     {"__kt__::println",       TL_KT_UTIL}},
  {"print",       {"__kt__::print",         TL_KT_UTIL}},
  {"also",        {"__kt__::also",          TL_KT_UTIL}},
  {"add",         {"__kt__::container_add", TL_KT_UTIL}},
  {"Int",         {"int",                   TL_NONE}},
  {"Double",      {"double",                TL_NONE}},
  {"Float",       {"float",                 TL_NONE}},
  {"Bool",        {"bool",                  TL_NONE}},
  {"Char",        {"char",                  TL_NONE}},
  {"Byte",        {"byte",                  TL_NONE}},
  {"Short",       {"short",                 TL_NONE}},
  {"Long",        {"long",                  TL_NONE}},
  {"Unit",        {"void",                  TL_NONE}},
  {"Byte",        {"char",                  TL_NONE}},
  {"UInt",        {"unsigned int",          TL_NONE}},
  {"ULong",       {"unsigned long",         TL_NONE}},
  {"UShort",      {"unsigned short",        TL_NONE}},
  {"UByte",       {"unsigned char",         TL_NONE}},
  {"UChar",       {"unsigned char",         TL_NONE}},
  {"size",        {"size()",                TL_NONE}}, // .size to .size()
};

const unordered_map<string, KtUtil> KT_UTILS {
  {"Scanner", {"kt_adapter/scanner.cpp", {"iostream", "string"}}},
  {"println", {"kt_adapter/println.cpp", {"iostream"}}},
  {"print", {"kt_adapter/print.cpp", {"iostream"}}},
  {"add", {"kt_adapter/container_add.cpp", {"vector", "unordered_set"}}},
  {"also", {"kt_adapter/obj_also.cpp"}},
  {"also", {"kt_adapter/obj_let.cpp"}},
};

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

string Transpiler::translateId(const string& in) {
  if (ID_TRANSLATE.find(in) != ID_TRANSLATE.end()) {
    TranslatedIdentifier t = ID_TRANSLATE.at(in);
    if (t.level > translationLevel.top()) return replaceAll(in, "`", "");
    if (t.needsImport) imports.insert(t.name);
    if (t.level == TL_KT_UTIL && KT_UTILS.find(in) != KT_UTILS.end()) {
      ktUtils.insert(in);
      imports.insert(KT_UTILS.at(in).imports.begin(), KT_UTILS.at(in).imports.end());
    }
    return (t.needsImport ? "std::" : "") + t.name;
  }
  return replaceAll(in, "`", "");
}

string getCallNodeMethodName(CallNode* in) {
  if (in->toCall->type == N_VAR_ACCESS) {
    auto* van = (VarAccessNode*) in->toCall;
    return van->methodName->value;
  }
  return "Can't handle";
}

const unordered_set<string> TOP_LEVEL_MANGLE {
  "println", "print", "Scanner"
};

const unordered_set<string> MEMBER_MANGLE {
  "add", "also"
};

bool shouldMangleCallNode(CallNode* in) {
  if (in->toCall->type != N_VAR_ACCESS) return false;
  string name = getCallNodeMethodName(in);
  if (TOP_LEVEL_MANGLE.find(name) != TOP_LEVEL_MANGLE.end()) return true;
  if (((VarAccessNode*) in->toCall)->members.empty()) return false;
  return MEMBER_MANGLE.find(name) != MEMBER_MANGLE.end();
}

const unordered_set<string> VAR_ACCESS_MANGLE {
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
      auto newArgs = vector<Node*>();
      auto membersMinusLast = vector<Token*>(van->members.begin(), van->members.end() - 1);
      newArgs.push_back(new VarAccessNode(van->idTok, membersMinusLast));
      newArgs.insert(newArgs.end(), in->args.begin(), in->args.end());
      return new CallNode(new VarAccessNode(van->methodName), in->generics, newArgs);
    }
    if (van->parent) {
      auto newArgs = vector<Node*>();
      auto membersMinusLast = vector<Token*>(van->members.begin(), van->members.end() - 1);
      newArgs.push_back(new VarAccessNode(nullptr, membersMinusLast, van->parent));
      newArgs.insert(newArgs.end(), in->args.begin(), in->args.end());
      return new CallNode(new VarAccessNode(van->methodName), in->generics, newArgs);
    }
  }
  throw runtime_error("Can't mangle call node");
}

Transpiler::Transpiler(ParseResult* ast) : ast(ast) {
  translationLevel.push(TL_IDENTIFIERS);
  typeAbove.push(N_LIST);
}

string Transpiler::transpile() {
  recurse(ast->node, out);

  stringstream importsOut;
  for (const auto& i : imports) {
    importsOut << "#include <" << i << ">" << '\n';
  }
  importsOut << '\n';

  string mini = buildKtUtils();
  replaceAllInPlace(mini, "\n", "");
  replaceAllInPlace(mini, "\r", "");
  replaceAllInPlace(mini, "+ ", "+");
  replaceAllInPlace(mini, " +", "+");
  replaceAllInPlace(mini, " -", "-");
  replaceAllInPlace(mini, "- ", "-");
  replaceAllInPlace(mini, "* ", "*");
  replaceAllInPlace(mini, " *", "*");
  replaceAllInPlace(mini, "/ ", "/");
  replaceAllInPlace(mini, " /", "/");
  replaceAllInPlace(mini, " =", "=");
  replaceAllInPlace(mini, "= ", "=");
  replaceAllInPlace(mini, " ,", ",");
  replaceAllInPlace(mini, ", ", ",");
  replaceAllInPlace(mini, "  ", "");
  replaceAllInPlace(mini, " &", "&");
  replaceAllInPlace(mini, "& ", "&");
  replaceAllInPlace(mini, " *", "*");
  replaceAllInPlace(mini, "* ", "*");
  replaceAllInPlace(mini, " {", "{");
  replaceAllInPlace(mini, "{ ", "{");
  replaceAllInPlace(mini, "} ", "}");
  replaceAllInPlace(mini, " }", "}");
  replaceAllInPlace(mini, " (", "(");
  replaceAllInPlace(mini, "( ", "(");
  replaceAllInPlace(mini, ") ", ")");
  replaceAllInPlace(mini, " )", ")");
  replaceAllInPlace(mini, "< ", "<");
  replaceAllInPlace(mini, " <", "<");
  replaceAllInPlace(mini, " >", ">");
  replaceAllInPlace(mini, "> ", ">");

  stringstream real;
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
    throw runtime_error("Expected binary operation");
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
      throw runtime_error("Expected range, until, or downto, got " + binOp->opTok->toString());
  }
  return spec;
}

void Transpiler::recurse(Node* in, stringstream& output, int nesting) {
  NodeType above = typeAbove.top();
  if (in->type != N_VAR_ACCESS)
    typeAbove.push(in->type);
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
        recurse(varDecl->value, out, varDecl->value->type == N_LAMBDA ? nesting : 0);
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
          recurse(call->generics[i], out, 0);
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
        string op = spec.inclusive ? (spec.down ? ">=" : "<=") : (spec.down ? ">" : "<");
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
    case N_MAP:
      break;
  }
  if (in->type != N_VAR_ACCESS)
    typeAbove.pop();
  output << out.str();
}

string readFile(const string& path) {
  ifstream file(path);
  stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

string Transpiler::buildKtUtils() {
  stringstream output;
  output << "namespace __kt__ {\n";
  for (const string& s : ktUtils) {
    output << readFile(KT_UTILS.at(s).contents) << endl;
  }
  output << '\n';
  output << "}";
  return output.str();
}
