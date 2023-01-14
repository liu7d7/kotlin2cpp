//
// Created by Aadi and Michael on 7/24/22.
//

#ifndef AT_NODE_H
#define AT_NODE_H

#include <Position.h>

enum NodeType {
  N_NUMBER,
  N_STRING,
  N_BIN_OP,
  N_UN_OP,
  N_VAR_ASSIGN,
  N_VAR_ACCESS,
  N_IF,
  N_FOR,
  N_FUNC_DEF,
  N_CALL,
  N_WHILE,
  N_LIST,
  N_RETURN,
  N_BREAK,
  N_CONTINUE,
  N_PACKAGE,
  N_CLASS,
  N_IDX,
  N_IMPORT,
  N_MAP,
  N_VAR_DECL,
  N_ARG,
  N_DATACLASS,
  N_TYPE,
  N_DO_WHILE
};

[[nodiscard]] string NodeType_toString(NodeType in);

class Node {
public:
  NodeType type;
  Position* posStart, * posEnd;

  Node(NodeType type);

  virtual string toString() const = 0;
};

#endif //AT_NODE_H
