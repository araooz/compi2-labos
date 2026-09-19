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

Program* Parser::parseProgram() {
    Program* programa = new Program();
    programa->cuerpo.push_back(parseStm());
    while(match(Token::SEMICOLON)){
        if (isAtEnd()) break;
        programa->cuerpo.push_back(parseStm());
    }
    if (!isAtEnd()) {
        throw runtime_error("Error sintáctico");
    }
    cout << "Parseo exitoso" << endl;
    return programa;
}

Body* Parser::parseBody(){
    Body* body = new Body();
    body->list_stm.push_back(parseStm());
    while(match(Token::SEMICOLON)){
        if (check(Token::ENDIF) || check(Token::ELIF) || check(Token::ELSE) 
            || check(Token::WHILE) || check(Token::ENDWHILE)
            || check(Token::CASE) || check(Token::DEFAULT) || check(Token::ENDSWITCH)) {
            break;
        }
        body->list_stm.push_back(parseStm());
    }
    return body;
}

Stm* Parser::parseStm(){
    if (match(Token::ID))
    {
        AssignStatement* stm = new AssignStatement();
        stm->variable = previous->text;
        match(Token::ASSIGN);
        stm->valor =  parseCE();
        return stm;
    }
    else if (match(Token::PRINT))
    {
        PrintStatement* stm = new PrintStatement();
        match(Token::LPAREN);
        stm->valor =  parseCE();
        match(Token::RPAREN);

        return stm;
    }
    else if (match(Token::IF))
    {
        IfStatement* stm = new IfStatement();
        stm->condition = parseCE();
        if (!match(Token::THEN)) {
            throw runtime_error("Error sintáctico: se esperaba 'then'");
        }
        stm->ifbody = parseBody();
        while (match(Token::ELIF)) {
            Exp* elifCond = parseCE();
            if (!match(Token::THEN)) {
                throw runtime_error("Error sintáctico: se esperaba 'then'");
            }
            Body* elifBody = parseBody();
            stm->elif_list.push_back(new ElifStatement(elifCond, elifBody));
        }
        if (match(Token::ELSE)) {
            stm->elsebody = parseBody();
        }
        if (!match(Token::ENDIF)) {
            throw runtime_error("Error sintáctico: se esperaba 'endif'");
        }
        return stm;
    }
    else if (match(Token::DO))
    {
        DoWhileStatement* stm = new DoWhileStatement();
        stm->body = parseBody();
        if (!match(Token::WHILE)) {
            throw runtime_error("Error sintáctico: se esperaba 'while'");
        }
        stm->condition = parseCE();
        return stm;
    }
    else if (match(Token::WHILE))
    {
        WhileStatement* stm = new WhileStatement();
        stm->condition = parseCE();
        if (!match(Token::DO)) {
            throw runtime_error("Error sintáctico: se esperaba 'do'");
        }
        stm->body = parseBody();
        if (!match(Token::ENDWHILE)) {
            throw runtime_error("Error sintáctico: se esperaba 'endwhile'");
        }
        return stm;
    }
    else if (match(Token::SWITCH))
    {
        SwitchStatement* stm = new SwitchStatement();
        stm->expr = parseCE();
        while (match(Token::CASE)) {
            Exp* caseVal = parseCE();
            Body* caseBody = parseBody();
            bool hasBreak = false;
            if (match(Token::BREAK)) {
                hasBreak = true;
            }
            stm->cases.push_back(new CaseStatement(caseVal, caseBody, hasBreak));
        }
        if (match(Token::DEFAULT)) {
            stm->defaultBody = parseBody();
        }
        if (!match(Token::ENDSWITCH)) {
            throw runtime_error("Error sintáctico: se esperaba 'endswitch'");
        }
        return stm;
    }
    else if (match(Token::BREAK))
    {
        return new BreakStatement();
    }
    else {
        throw runtime_error("Error sintáctico");
    }
    
}

// CExp → LOrExp
Exp* Parser::parseCE() {
    return parseLOrExp();
}

// LOrExp → LAndExp { 'or' LAndExp }*
Exp* Parser::parseLOrExp() {
    Exp* l = parseLAndExp();
    while (match(Token::OR)) {
        Exp* r = parseLAndExp();
        l = new BinaryExp(l, r, OR_OP);
    }
    return l;
}

// LAndExp → RelExp { 'and' RelExp }*
Exp* Parser::parseLAndExp() {
    Exp* l = parseRelExp();
    while (match(Token::AND)) {
        Exp* r = parseRelExp();
        l = new BinaryExp(l, r, AND_OP);
    }
    return l;
}

// RelExp → Expr { ('<' | '>' | '<=' | '>=' | '==' | '!=') Expr }*
Exp* Parser::parseRelExp() {
    Exp* l = parseExpr();
    while (match(Token::LT) || match(Token::GT) || match(Token::LE) 
           || match(Token::GE) || match(Token::EQ) || match(Token::NE)) {
        BinaryOp op;
        if (previous->type == Token::LT) op = LT_OP;
        else if (previous->type == Token::GT) op = GT_OP;
        else if (previous->type == Token::LE) op = LE_OP;
        else if (previous->type == Token::GE) op = GE_OP;
        else if (previous->type == Token::EQ) op = EQ_OP;
        else op = NE_OP;
        Exp* r = parseExpr();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Expr → Term { ('+' | '-') Term }*
Exp* Parser::parseExpr() {
    Exp* l = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseTerm();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Term → PowExp { ('*' | '/') PowExp }*
Exp* Parser::parseTerm() {
    Exp* l = parsePow();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parsePow();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// PowExp → Factor [ '**' Factor ]
Exp* Parser::parsePow() {
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
    else if (match(Token::TRUE)) {
        return new BoolExp(true);
    }
    else if (match(Token::FALSE)) {
        return new BoolExp(false);
    }
    else if (match(Token::ID)) {
        string va  = previous->text;
        return new IdExp(va);
    } 
    else if (match(Token::LPAREN))
    {
        e = parseCE();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::SQRT))
    {   
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        return new SqrtExp(e);
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
