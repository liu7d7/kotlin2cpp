//
// Created by richard may clarkson on 14/01/2023.
//

#ifndef KT2CPP_TRANSPILER_H
#define KT2CPP_TRANSPILER_H

#include <unordered_map>
#include <fstream>
#include "stack"
#include "ParseResult.h"
#include "unordered_set"
#include "tsl/ordered_set.h"

enum TranslateLevel {
  TL_NONE, TL_IDENTIFIERS, TL_KT_UTIL
};

const std::unordered_set<NodeType> NO_SEMICOLONS{
  N_IF, N_WHILE, N_FOR, N_FUNC_DEF, N_PACKAGE
};

const std::unordered_set<NodeType> DOUBLE_NEWLINE{
  N_FUNC_DEF, N_PACKAGE, N_CLASS, N_DATACLASS, N_IF, N_WHILE, N_FOR, N_DO_WHILE, N_TYPEALIAS
};

struct TranslatedIdentifier {
  std::string name;
  TranslateLevel level;
  bool needsImport = false;
};

struct KtUtil {
  std::string contents;
  std::unordered_set<std::string> imports{};
  std::unordered_set<std::string> ktUtils{};
};

const std::unordered_map<std::string, TranslatedIdentifier> ID_TRANSLATE{
  {"HashMap",     {"unordered_map",         TL_IDENTIFIERS, true}},
  {"HashSet",     {"unordered_set",         TL_IDENTIFIERS, true}},
  {"ArrayList",   {"vector",                TL_IDENTIFIERS, true}},
  {"MutableList", {"vector",                TL_IDENTIFIERS, true}},
  {"LinkedList",  {"list",                  TL_IDENTIFIERS, true}},
  {"Pair",        {"pair",                  TL_IDENTIFIERS, true}},
  {"String",      {"string",                TL_IDENTIFIERS, true}},
  {"System",      {"__kt__::System",        TL_KT_UTIL}},
  {"Scanner",     {"__kt__::Scanner",       TL_KT_UTIL}},
  {"println",     {"__kt__::println",       TL_KT_UTIL}},
  {"print",       {"__kt__::print",         TL_KT_UTIL}},
  {"also",        {"__kt__::also",          TL_KT_UTIL}},
  {"add",         {"__kt__::container_add", TL_KT_UTIL}},
  {"let",         {"__kt__::let",           TL_KT_UTIL}},
  {"map",         {"__kt__::map",           TL_KT_UTIL}},
  {"toString",    {"__kt__::toString",      TL_KT_UTIL}},
  {"Array",       {"__kt__::Array_ctor",    TL_KT_UTIL}},
  {"sort",        {"__kt__::sort",          TL_KT_UTIL}},
  {"sorted",      {"__kt__::sorted",        TL_KT_UTIL}},
  {"sortBy",      {"__kt__::sortBy",        TL_KT_UTIL}},
  {"sortedBy",    {"__kt__::sortedBy",      TL_KT_UTIL}},
  {"Int",         {"int",                   TL_NONE}},
  {"Double",      {"double",                TL_NONE}},
  {"Float",       {"float",                 TL_NONE}},
  {"Bool",        {"bool",                  TL_NONE}},
  {"Char",        {"char",                  TL_NONE}},
  {"Byte",        {"byte",                  TL_NONE}},
  {"Short",       {"short",                 TL_NONE}},
  {"Long",        {"long long",             TL_NONE}},
  {"Unit",        {"void",                  TL_NONE}},
  {"Byte",        {"char",                  TL_NONE}},
  {"UInt",        {"unsigned int",          TL_NONE}},
  {"ULong",       {"unsigned long long",    TL_NONE}},
  {"UShort",      {"unsigned short",        TL_NONE}},
  {"UByte",       {"unsigned char",         TL_NONE}},
  {"UChar",       {"unsigned char",         TL_NONE}},
  {"size",        {"size()",                TL_NONE}}, // .size to .size()
};

const std::unordered_map<std::string, KtUtil> KT_UTILS{
  {"Scanner",  {"kt_adapter/scanner.cpp",       {"iostream", "string"}}},
  {"println",  {"kt_adapter/println.cpp",       {"iostream"},                                             {"toString"}}},
  {"print",    {"kt_adapter/print.cpp",         {"iostream"},                                             {"toString"}}},
  {"add",      {"kt_adapter/container_add.cpp", {"vector",   "unordered_set"}}},
  {"also",     {"kt_adapter/obj_also.cpp"}},
  {"let",      {"kt_adapter/obj_let.cpp"}},
  {"toString", {"kt_adapter/to_string.cpp",     {"vector",   "string", "unordered_map", "unordered_set"}, {"Array"}}},
  {"Array",    {"kt_adapter/array.cpp"}},
  {"sort",     {"kt_adapter/sort.cpp",          {"algorithm"}}},
  {"sorted",   {"kt_adapter/sorted.cpp",        {"algorithm"}}},
  {"sortBy",   {"kt_adapter/sort_by.cpp",       {"algorithm"}}},
  {"sortedBy", {"kt_adapter/sorted_by.cpp",     {"algorithm"}}},
  {"map",      {"kt_adapter/container_map.cpp", {"vector",   "unordered_set"},                            {"Array"}}}
};

const std::unordered_set<std::string> TOP_LEVEL_MANGLE{
  "println", "print", "Scanner", "Array", "sort", "sortBy", "sorted", "sortedBy"
};

const std::unordered_set<std::string> MEMBER_MANGLE{
  "add", "also", "let", "map", "toString"
};

const std::unordered_set<std::string> VAR_ACCESS_MANGLE{
  "System"
};

class Transpiler {
public:

  explicit Transpiler(ParseResult* ast);

  [[nodiscard]] std::string transpile();

  [[nodiscard]] std::string translateId(const std::string& in);

  template<std::ranges::range T>
  [[nodiscard]] std::string buildKtUtils(const T& ktUtils, int depth = 0) {
    std::stringstream output;
    if (depth == 0)
      output << "namespace __kt__ {\n";
    for (const std::string& s : ktUtils) {
      if (addedKtUtils.contains(s)) continue;
      auto util = KT_UTILS.at(s);
      if (!util.ktUtils.empty()) {
        output << buildKtUtils(util.ktUtils, depth + 1);
      }
      imports.insert(util.imports.begin(), util.imports.end());
      output << readFile(util.contents) << '\n';
      addedKtUtils.insert(s);
    }
    if (depth == 0) {
      output << '\n';
      output << "}";
    }
    return output.str();
  }

  void recurse(Node* in, std::stringstream& output, int nesting = 0);

private:
  ParseResult* ast;
  std::stringstream out;
  std::unordered_set<std::string> imports;
  tsl::ordered_set<std::string> ktUtils;
  std::unordered_set<std::string> addedKtUtils;
  std::stack<TranslateLevel> translationLevel;
  std::stack<NodeType> typeAbove;

  std::string readFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }
};

#endif //KT2CPP_TRANSPILER_H
