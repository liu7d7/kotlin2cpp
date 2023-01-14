cmake_minimum_required(VERSION 3.21)
project(kt2cpp)

set(CMAKE_CXX_STANDARD 20)
# Set flags for release mode
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(
    SRC_FILES
    include/Lexer.h src/Lexer.cpp include/Token.h src/Token.cpp src/Position.cpp include/Error.h src/Error.cpp include/Parser.h include/Nodes/Node.h
    src/Node.cpp include/Nodes/NumberNode.h src/NumberNode.cpp include/ParseResult.h src/ParseResult.cpp src/Parser.cpp include/Nodes/UnaryOpNode.h src/UnaryOpNode.cpp
    include/Nodes/BinaryOpNode.h src/BinaryOpNode.cpp include/Nodes/VarAssignNode.h src/VarAssignNode.cpp src/VarAccessNode.cpp include/Nodes/VarAccessNode.h
    include/Nodes/IfNode.h src/IfNode.cpp include/Nodes/ForNode.h src/ForNode.cpp include/Nodes/FuncDefNode.h src/FuncDefNode.cpp include/Nodes/CallNode.h src/CallNode.cpp
    include/Nodes/WhileNode.h src/WhileNode.cpp include/Nodes/StringNode.h src/StringNode.cpp include/Nodes/ListNode.h src/ListNode.cpp include/Nodes/ReturnNode.h
    src/ReturnNode.cpp include/Nodes/BreakNode.h include/Nodes/ContinueNode.h src/ContinueNode.cpp src/BreakNode.cpp include/Nodes/PackageNode.h
    src/PackageNode.cpp include/Nodes/ClassNode.h src/ClassNode.cpp include/Nodes/IndexNode.h src/IndexNode.cpp include/Nodes/ImportNode.h src/ImportNode.cpp
    include/Nodes/MapNode.h src/MapNode.cpp include/Position.h  include/Global.h include/Nodes/VarDeclNode.h src/VarDeclNode.cpp include/Nodes/ArgNode.h src/ArgNode.cpp
    include/Nodes/DataclassNode.h src/DataclassNode.cpp include/Transpiler.h src/Transpiler.cpp include/Nodes/TypeNode.h src/TypeNode.cpp
)


include_directories(include)

add_executable(kt2cpp main.cpp ${SRC_FILES})