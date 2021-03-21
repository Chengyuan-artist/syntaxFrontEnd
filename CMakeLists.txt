cmake_minimum_required(VERSION 3.17)
project(syntaxFrontEnd)

set(CMAKE_CXX_STANDARD 11)


add_executable(syntax src/main.cpp src/node.hpp src/node.cpp src/parser.cpp src/parser.hpp src/lexical.hpp src/lexical.cpp src/token.cpp src/token.hpp src/tokenlist.cpp src/tokenlist.hpp)