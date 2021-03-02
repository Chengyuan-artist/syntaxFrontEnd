cmake_minimum_required(VERSION 3.17)
project(syntaxFrontEnd)

set(CMAKE_CXX_STANDARD 11)

add_executable(syntaxFrontEnd main.cpp lexical.cpp parsing.cpp syntax.h)