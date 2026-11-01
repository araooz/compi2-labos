#ifndef VISITOR_H
#define VISITOR_H

#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include "ast.h"
#include "type.h"
#include "environment.h"

class TypeChecker;

// Tamaño de cada valor: int, bool, punteros y pilas ocupan 8 bytes (en la pila y en el heap)
const int TAM_VARIABLE = 8;

// Una variable stack guarda la dirección de una cabecera en el heap: [tope, tamaño].
// Los elementos forman una lista enlazada de nodos: [valor, siguiente].
// Pasar una pila a una función comparte la misma pila (se copia la dirección).
const int TAM_CABECERA_PILA = 16;
const int TAM_NODO_PILA = 16;

// Interfaz Visitor: una operación visit por cada nodo del AST.
// Las expresiones devuelven Type; las sentencias y declaraciones, void.
class Visitor {
public:
    // Expresiones
    virtual Type visit(BinaryExp* exp) = 0;
    virtual Type visit(NumberExp* exp) = 0;
    virtual Type visit(IdExp* exp) = 0;
    virtual Type visit(IfExp* exp) = 0;
    virtual Type visit(FCallExp* exp) = 0;
    virtual Type visit(IndexExp* exp) = 0;
    virtual Type visit(DerefExp* exp) = 0;
    virtual Type visit(AddrExp* exp) = 0;
    virtual Type visit(MallocExp* exp) = 0;
    virtual Type visit(StackOpExp* exp) = 0;
    // Sentencias
    virtual void visit(AssignStmt* stm) = 0;
    virtual void visit(StackStmt* stm) = 0;
    virtual void visit(StoreStmt* stm) = 0;
    virtual void visit(FreeStmt* stm) = 0;
    virtual void visit(PrintStmt* stm) = 0;
    virtual void visit(IfStmt* stm) = 0;
    virtual void visit(WhileStmt* stm) = 0;
    virtual void visit(DoWhileStmt* stm) = 0;
    virtual void visit(BreakStmt* stm) = 0;
    virtual void visit(ReturnStmt* stm) = 0;
    // Declaraciones y programa
    virtual void visit(VarDec* dec) = 0;
    virtual void visit(Body* body) = 0;
    virtual void visit(FunDec* fun) = 0;
    virtual void visit(Programa* program) = 0;
};

// ==================== GencodeVisitor ====================

// Visitor que genera código ensamblador x86-64 a partir del AST.
// Convención de llamada (System V): los argumentos van en
// %rdi, %rsi, %rdx, %rcx, %r8, %r9 y el resultado vuelve en %rax.
class GencodeVisitor : public Visitor {
public:
    ostream& out;
    TypeChecker* checker;                    // ya ejecutado: dice cuántas variables tiene cada función
    int contador = 1;                        // siguiente posición libre en la pila de la función actual
    Environment<int> posicion;               // posición en la pila de cada variable / parámetro
    Environment<Type> tipos;                 // tipo de cada variable (y su último valor)
    int etiquetas = 0;                       // contador para generar etiquetas únicas
    vector<string> finDeBucle;               // etiqueta de salida de cada bucle abierto (para break)
    unordered_map<string, FunDec*> funciones; // funciones del programa, por nombre
    string funcionActual;                    // función que se está generando (para return)
    int pila = 0;                            // pushq pendientes (para alinear la pila en cada call)
    bool usaPilaVacia = false;               // true si hay pop/top: se genera la rutina de error

    GencodeVisitor(ostream& out, TypeChecker* checker);
    void gencode(Programa* program);

    Type visit(BinaryExp* exp) override;
    Type visit(NumberExp* exp) override;
    Type visit(IdExp* exp) override;
    Type visit(IfExp* exp) override;
    Type visit(FCallExp* exp) override;
    Type visit(IndexExp* exp) override;
    Type visit(DerefExp* exp) override;
    Type visit(AddrExp* exp) override;
    Type visit(MallocExp* exp) override;
    Type visit(StackOpExp* exp) override;
    void visit(AssignStmt* stm) override;
    void visit(StackStmt* stm) override;
    void visit(StoreStmt* stm) override;
    void visit(FreeStmt* stm) override;
    void visit(PrintStmt* stm) override;
    void visit(IfStmt* stm) override;
    void visit(WhileStmt* stm) override;
    void visit(DoWhileStmt* stm) override;
    void visit(BreakStmt* stm) override;
    void visit(ReturnStmt* stm) override;
    void visit(VarDec* dec) override;
    void visit(Body* body) override;
    void visit(FunDec* fun) override;
    void visit(Programa* program) override;

private:
    Type direccion(Exp* lvalue);             // deja en %rax la dirección de x, a[i] o *p
    void llamar(const string& funcion);      // call con la pila alineada a 16 bytes
};


class TypeChecker : public Visitor {
public:
    Environment<Type> tipos;                  // tipo declarado de cada variable / parámetro
    unordered_map<string, FunDec*> funciones; // firma de cada función, por nombre
    FunDec* funcionActual = nullptr;          // función que se está verificando
    int bucles = 0;                           // cantidad de bucles abiertos (para break)

    // Conteos que usa el GencodeVisitor para reservar la pila
    int numFunciones = 0;                     // funciones del programa
    unordered_map<string, int> numVariables;  // por función: parámetros + locales
    int espacioPila(const string& funcion);   // bytes a reservar en la pila

    void check(Programa* program);

    Type visit(BinaryExp* exp) override;
    Type visit(NumberExp* exp) override;
    Type visit(IdExp* exp) override;
    Type visit(IfExp* exp) override;
    Type visit(FCallExp* exp) override;
    Type visit(IndexExp* exp) override;
    Type visit(DerefExp* exp) override;
    Type visit(AddrExp* exp) override;
    Type visit(MallocExp* exp) override;
    Type visit(StackOpExp* exp) override;
    void visit(AssignStmt* stm) override;
    void visit(StackStmt* stm) override;
    void visit(StoreStmt* stm) override;
    void visit(FreeStmt* stm) override;
    void visit(PrintStmt* stm) override;
    void visit(IfStmt* stm) override;
    void visit(WhileStmt* stm) override;
    void visit(DoWhileStmt* stm) override;
    void visit(BreakStmt* stm) override;
    void visit(ReturnStmt* stm) override;
    void visit(VarDec* dec) override;
    void visit(Body* body) override;
    void visit(FunDec* fun) override;
    void visit(Programa* program) override;

private:
    void esperarBool(Exp* condicion, const string& donde);
};

#endif // VISITOR_H
