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

Parser::~Parser() {
    if (previous) delete previous;
    if (current) delete current;
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    return current->type == ttype;
}

void Parser::consume(Token::Type ttype) {
    if (!match(ttype)) {
        throw runtime_error("Error sintáctico");
    }
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
    Exp* l = parseCE();
    return l;
}

Exp* Parser::parseCE() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseT() {
    Exp* l = parseF();
    if (match(Token::POW)) {
        BinaryOp op = POW_OP;
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseF() {
    Exp* e; 
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    } else if (match(Token::FLOAT)) {
        return new FloatExp(stod(previous->text));
    } else if (match(Token::LPAREN)) {
        e = parseCE();
        consume(Token::RPAREN);
        return e;
    } else if(match(Token::ABS)) {
        consume(Token::LPAREN);
        e = parseCE();
        consume(Token::RPAREN);
        return new AbsExp(e);
    } else if (match(Token::MAX)) {
        consume(Token::LPAREN);
        list<Exp*> args;

        args.push_back(parseCE());
        consume(Token::COMMA);
        args.push_back(parseCE());

        while (match(Token::COMMA)) {
            args.push_back(parseCE());
        }

        consume(Token::RPAREN);
        return new MaxExp(args);
    } else if (match(Token::MIN)) {
        consume(Token::LPAREN);
        list<Exp*> args;

        args.push_back(parseCE());
        consume(Token::COMMA);
        args.push_back(parseCE());

        while (match(Token::COMMA)) {
            args.push_back(parseCE());
        }

        consume(Token::RPAREN);
        return new MinExp(args);
   } else if (match(Token::SQRT)) {   
        consume(Token::LPAREN);
        e = parseCE();
        consume(Token::RPAREN);
        return new SqrtExp(e);
    } else if (match(Token::ID)) {   
        return new IdExp(previous->text);
    } else {
        throw runtime_error("Error sintáctico");
    }
}
