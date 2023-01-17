//
// Created by Aadi and Michael on 7/24/22.
//

#include <Nodes/Node.h>

std::string NodeType_toString(NodeType in) {
  const std::string strs[] {
    "N_NUMBER", "N_STRING", "N_BIN_OP", "N_UN_OP", "N_VAR_ASSIGN",
    "N_VAR_ACCESS", "N_IF", "N_FOR", "N_FUNC_DEF", "N_CALL",
    "N_WHILE", "N_LIST", "N_RETURN", "N_BREAK", "N_CONTINUE",
    "N_PACKAGE", "N_CLASS", "N_IDX", "N_IMPORT", "N_MAP",
    "N_VAR_DECL", "N_ARG", "N_DATACLASS", "N_TYPE", "N_DO_WHILE",
    "N_TYPEALIAS", "N_LAMBDA", "N_INTERPOLATED_STRING"
  };
  return strs[in];
}

Node::Node(NodeType type) {
  this->type = type;
}

std::string Node::toString() const {
  return std::to_string(type);
}