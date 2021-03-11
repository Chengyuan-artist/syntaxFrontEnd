cmake_minimum_required(VERSION 3.17)
project(syntaxFrontEnd)

set(CMAKE_CXX_STANDARD 11)


add_executable(refactor refactor/lexical_re.cpp refactor/lexical_re.hpp refactor/main_re.cpp refactor/node_re.hpp refactor/node_re.cpp refactor/parser_re.cpp refactor/parser_re.hpp)