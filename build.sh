#! /bin/bash
g++ -c LexicalAnalyzer.cpp -o LexicalAnalyzer.o -O2
g++ -c SyntaxAnalyzer.cpp -o SyntaxAnalyzer.o -O2
g++ -c publicVariates.cpp -o publicVariates.o -O2
g++ -c Lab1.cpp -o Lab1.o -O2

g++ LexicalAnalyzer.o SyntaxAnalyzer.o publicVariates.o Lab1.o -o Compiler -lm

g++ -c Lab2.cpp -o Lab2.o -O2
g++ Lab2.o -o Interpreter -lm
