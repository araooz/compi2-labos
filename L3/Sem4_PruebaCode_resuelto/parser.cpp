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
    delete previous;
    delete current;
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

void Parser::expect(Token::Type ttype, const string& message) {
    if (!match(ttype)) throw runtime_error(message);
}


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* programa = new Program();
    programa->cuerpo.push_back(parseStm());
    while(match(Token::SEMICOLON)){
        programa->cuerpo.push_back(parseStm());
    }
    if (!isAtEnd()) {
        throw runtime_error("Error sintáctico");
    }
    cout << "Parseo exitoso" << endl;
    return programa;
}

Stm* Parser::parseStm(){
    if (match(Token::ID))
    {
        AssignStatement* stm = new AssignStatement();
        stm->variables.push_back(previous->text);
        while (match(Token::COMMA)) {
            expect(Token::ID, "Se esperaba un identificador después de ','");
            stm->variables.push_back(previous->text);
        }
        expect(Token::ASSIGN, "Se esperaba '=' en la asignación");
        stm->valores.push_back(parseCE());
        while (match(Token::COMMA)) {
            stm->valores.push_back(parseCE());
        }
        if (stm->variables.size() != stm->valores.size()) {
            delete stm;
            throw runtime_error("La cantidad de identificadores y expresiones no coincide");
        }
        return stm;
    }
    else if (match(Token::PRINT))
    {
        PrintStatement* stm = new PrintStatement();
        expect(Token::LPAREN, "Se esperaba '(' después de print");
        stm->valores.push_back(parseCE());
        while (match(Token::COMMA)) {
            stm->valores.push_back(parseCE());
        }
        expect(Token::RPAREN, "Se esperaba ')' al final de print");

        return stm;
    }

    else {
        throw runtime_error("Error sintáctico");
    }
    
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
    else if (match(Token::ID)) {
        string va  = previous->text;
        return new IdExp(va);
    } 
    else if (match(Token::LPAREN))
    {
        e = parseCE();
        expect(Token::RPAREN, "Se esperaba ')'");
        return e;
    }
    else if (match(Token::SQRT))
    {   
        expect(Token::LPAREN, "Se esperaba '(' después de sqrt");
        e = parseCE();
        expect(Token::RPAREN, "Se esperaba ')' después del argumento de sqrt");
        return new SqrtExp(e);
    }
    else if (match(Token::MIN) || match(Token::MAX))
    {
        FunctionOp op = previous->type == Token::MIN ? MIN_OP : MAX_OP;
        expect(Token::LPAREN, "Se esperaba '(' después de min/max");
        Exp* left = parseCE();
        expect(Token::COMMA, "Se esperaba ',' entre los argumentos de min/max");
        Exp* right = parseCE();
        expect(Token::RPAREN, "Se esperaba ')' después de min/max");
        return new FunctionExp(left, right, op);
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
