#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}
Stm::~Stm() {}
string Exp::binopToChar(BinaryOp op) {
  switch (op) {
  case PLUS_OP:
    return "+";
  case MINUS_OP:
    return "-";
  case MUL_OP:
    return "*";
  case DIV_OP:
    return "/";
  case POW_OP:
    return "**";
  default:
    return "?";
  }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp *l, Exp *r, BinaryOp o) : left(l), right(r), op(o) {}

BinaryExp::~BinaryExp() {
  delete left;
  delete right;
}

// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}

// ------------------ SqrtExp ------------------
SqrtExp::SqrtExp(Exp *v) : value(v) {}

SqrtExp::~SqrtExp() { delete value; }

FunctionExp::FunctionExp(Exp *l, Exp *r, FunctionOp o)
    : left(l), right(r), op(o) {}

FunctionExp::~FunctionExp() {
  delete left;
  delete right;
}

IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}

AssignStatement::~AssignStatement() {
  for (Exp *valor : valores)
    delete valor;
}

PrintStatement::~PrintStatement() {
  for (Exp *valor : valores)
    delete valor;
}

Program::~Program() {
  for (Stm *stm : cuerpo)
    delete stm;
}
