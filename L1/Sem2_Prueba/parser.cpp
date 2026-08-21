#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"
 
using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
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
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Exp* Parser::parseProgram() {
    Exp* ast = parseP();
    if (!isAtEnd()) {
        throw runtime_error("Error sintáctico");
    }
    cout << "Parseo exitoso" << endl;
    return ast;
}


Exp* Parser::parseP() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        if(previous->type == Token::PLUS) {
            BinaryOp op =  PLUS_OP;
            Exp* r = parseE();
            l = new BinaryExp(l, r, op);
        } else {
            BinaryOp op =  MINUS_OP;
            Exp* r = parseE();
            l = new BinaryExp(l, r, op);
        }
    }
    return l;
}

Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::MUL) || match(Token::DIV)) {
        if(previous->type == Token::MUL) {
            BinaryOp op =  MUL_OP ;
            Exp* r = parseF();
            l = new BinaryExp(l, r, op);
        } else {
            BinaryOp op =  DIV_OP ;
            Exp* r = parseF();
            l = new BinaryExp(l, r, op);
        }
    }
    return l;
}

Exp* Parser::parseT() {
    Exp* l = parseF();
    while (match(Token::POW)) {
        BinaryOp op =  POW_OP ;
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseF() {
    Exp* e; 
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }

    if (match(Token::ID)) {
        return new IDExp(previous->text);
    }

    else if (match(Token::SQRT)) {
        if (!match(Token::LPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba '(' después de 'sqrt'");
        }
        e = parseP();
        if (!match(Token::RPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba ')'");
        }
        return e;
    }

    else if (match(Token::LOG)) {
        if (!match(Token::LPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba '(' después de 'log'");
        }
        e = parseP();
        if (!match(Token::RPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba ')'");
        }
        return e;
    }

    else if (match(Token::POW2)) {
        if (!match(Token::LPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba '(' después de 'pow2'");
        }
        
        Exp* base = parseP();

        if (!match(Token::COMMA)) {
            throw runtime_error("Error sintáctico: se esperaba ',' después de la base en 'pow2'");
        }

        Exp* exponent = parseP();

        if (!match(Token::RPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba ')' después del exponente en 'pow2'");
        }
        return new BinaryExp(base, exponent, POW_OP);
    }

    else if (match(Token::LPAREN)) {
        e = parseP();
        if (!match(Token::RPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba ')'");
        }
        return e;
    }

    else {
        throw runtime_error("Error sintáctico");
    }
}
