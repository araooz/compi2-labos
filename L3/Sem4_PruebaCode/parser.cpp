#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "token.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner *sc) : scanner(sc) {
  previous = nullptr;
  current = scanner->nextToken();
  if (current->type == Token::ERR) {
    throw runtime_error("Error léxico");
  }
}

bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype) {
  if (isAtEnd())
    return false;
  return current->type == ttype;
}

bool Parser::advance() {
  if (!isAtEnd()) {
    Token *temp = current;
    if (previous)
      delete previous;
    current = scanner->nextToken();
    previous = temp;

    if (check(Token::ERR)) {
      throw runtime_error("Error lexico");
    }
    return true;
  }
  return false;
}

bool Parser::isAtEnd() { return (current->type == Token::END); }

// =============================
// Reglas gramaticales
// =============================

Program *Parser::parseProgram() {
  Program *programa = new Program();
  programa->cuerpo.push_back(parseStm());
  while (match(Token::SEMICOLON)) {
    programa->cuerpo.push_back(parseStm());
  }
  if (!isAtEnd()) {
    throw runtime_error("Error sintáctico");
  }
  cout << "Parseo exitoso" << endl;
  return programa;
}

Stm *Parser::parseStm() {
  if (match(Token::ID)) {
    AssignStatement *stm = new AssignStatement();
    stm->variables.push_back(previous->text);
    while (match(Token::COMMA)) {
      stm->variables.push_back(previous->text);
    }
    stm->valores.push_back(parseCE());
    while (match(Token::COMMA)) {
      stm->valores.push_back(parseCE());
    }
    if (stm->variables.size() != stm->valores.size()) {
      delete stm;
      throw runtime_error(
          "La cantidad de identificadores y expresiones no coincide");
    }
    return stm;
  } else if (match(Token::PRINT)) {
    PrintStatement *stm = new PrintStatement();
    stm->valores.push_back(parseCE());
    while (match(Token::COMMA)) {
      stm->valores.push_back(parseCE());
    }

    return stm;
  }

  else {
    throw runtime_error("Error sintáctico");
  }
}

Exp *Parser::parseCE() {
  Exp *l = parseE();
  while (match(Token::PLUS) || match(Token::MINUS)) {
    BinaryOp op;
    if (previous->type == Token::PLUS) {
      op = PLUS_OP;
    } else {
      op = MINUS_OP;
    }
    Exp *r = parseE();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseE() {
  Exp *l = parseT();
  while (match(Token::MUL) || match(Token::DIV)) {
    BinaryOp op;
    if (previous->type == Token::MUL) {
      op = MUL_OP;
    } else {
      op = DIV_OP;
    }
    Exp *r = parseT();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseT() {
  Exp *l = parseF();
  if (match(Token::POW)) {
    BinaryOp op = POW_OP;
    Exp *r = parseF();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseF() {
  Exp *e;
  if (match(Token::NUM)) {
    return new NumberExp(stoi(previous->text));
  } else if (match(Token::ID)) {
    string va = previous->text;
    return new IdExp(va);
  } else if (match(Token::LPAREN)) {
    e = parseCE();
    return e;
  } else if (match(Token::SQRT)) {
    e = parseCE();
    return new SqrtExp(e);
  } else if (match(Token::MIN) || match(Token::MAX)) {
    FunctionOp op = previous->type == Token::MIN ? MIN_OP : MAX_OP;
    Exp *left = parseCE();
    Exp *right = parseCE();
    return new FunctionExp(left, right, op);
  } else {
    throw runtime_error("Error sintáctico");
  }
}
