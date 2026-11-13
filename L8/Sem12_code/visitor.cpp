#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "ast.h"
#include "visitor.h"

using namespace std;

// Registros donde van los argumentos de una llamada (convención System V)
static const char* REG_ARGS[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

// Tipo (sin valor concreto) que corresponde a un nombre de tipo: int, bool, int*, int**, ...
static Type tipoDe(const string& nombre) {
    return Type::desdeNombre(nombre);
}

// Instrucción set que materializa el resultado de cada comparación (con signo)
static string instruccionSet(BinaryOp op) {
    switch (op) {
        case LE_OP: return "setle";
        case LT_OP: return "setl";
        case GT_OP: return "setg";
        case GE_OP: return "setge";
        case EQ_OP: return "sete";
        default:    return "setne";
    }
}

// Indica si la expresión designa una posición de memoria: x, a[i], *p o s.campo
static bool esLvalue(Exp* e) {
    return dynamic_cast<IdExp*>(e) || dynamic_cast<IndexExp*>(e) || dynamic_cast<DerefExp*>(e)
        || dynamic_cast<FieldAccessExp*>(e);
}

// Indica si la expresión es el literal 0, que vale como puntero nulo (p = 0, p != 0)
static bool esNulo(Exp* e) {
    NumberExp* n = dynamic_cast<NumberExp*>(e);
    return n && !n->esBool && n->value == 0;
}

// =====================================================
// ====================== accept =======================
// =====================================================

Type BinaryExp::accept(Visitor* visitor)  { return visitor->visit(this); }
Type NumberExp::accept(Visitor* visitor)  { return visitor->visit(this); }
Type IdExp::accept(Visitor* visitor)      { return visitor->visit(this); }
Type IfExp::accept(Visitor* visitor)      { return visitor->visit(this); }
Type FCallExp::accept(Visitor* visitor)   { return visitor->visit(this); }
Type IndexExp::accept(Visitor* visitor)   { return visitor->visit(this); }
Type DerefExp::accept(Visitor* visitor)   { return visitor->visit(this); }
Type AddrExp::accept(Visitor* visitor)    { return visitor->visit(this); }
Type MallocExp::accept(Visitor* visitor)  { return visitor->visit(this); }
Type StackOpExp::accept(Visitor* visitor) { return visitor->visit(this); }
Type FieldAccessExp::accept(Visitor* visitor) { return visitor->visit(this); }
Type SizeofExp::accept(Visitor* visitor)  { return visitor->visit(this); }

void AssignStmt::accept(Visitor* visitor)  { visitor->visit(this); }
void StackStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void StoreStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void FreeStmt::accept(Visitor* visitor)    { visitor->visit(this); }
void PrintStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void IfStmt::accept(Visitor* visitor)      { visitor->visit(this); }
void WhileStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void DoWhileStmt::accept(Visitor* visitor) { visitor->visit(this); }
void BreakStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void ReturnStmt::accept(Visitor* visitor)  { visitor->visit(this); }

void VarDec::accept(Visitor* visitor)   { visitor->visit(this); }
void Body::accept(Visitor* visitor)     { visitor->visit(this); }
void FunDec::accept(Visitor* visitor)   { visitor->visit(this); }
void Programa::accept(Visitor* visitor) { visitor->visit(this); }

// =====================================================
// ================== GencodeVisitor ===================
// =====================================================

// Crea el generador de código, que escribe el ensamblador en out.
// Necesita el TypeChecker ya ejecutado para saber cuánta pila reservar en cada función.
GencodeVisitor::GencodeVisitor(ostream& out, TypeChecker* checker) : out(out), checker(checker) {}

// Genera el código de todo el programa
void GencodeVisitor::gencode(Programa* program) {
    program->accept(this);
}

// Genera el código de left op right y deja el resultado en %rax
Type GencodeVisitor::visit(BinaryExp* exp) {
    Type l = exp->left->accept(this);
    out << "pushq %rax" << endl;
    pila++;
    Type r = exp->right->accept(this);
    out << "movq %rax, %rcx" << endl;
    out << "popq %rax" << endl;
    pila--;
    // Aritmética de punteros: p + i avanza i elementos, es decir, i * tamaño del elemento
    bool aritmeticaPunteros = l.isPtr() && (exp->op == PLUS_OP || exp->op == MINUS_OP);
    if (aritmeticaPunteros) {
        out << "imulq $" << tamTipo(l.apuntado()) << ", %rcx" << endl;
    }
    switch (exp->op) {
        case PLUS_OP:
            out << "addq %rcx, %rax" << endl;
            break;
        case MINUS_OP:
            out << "subq %rcx, %rax" << endl;
            break;
        case MUL_OP:
            out << "imulq %rcx, %rax" << endl;
            break;
        case DIV_OP:
            out << "cqto" << endl;                 // extiende el signo de %rax a %rdx
            out << "idivq %rcx" << endl;           // cociente en %rax
            break;
        case POW_OP: {
            // %rax = base ** exponente, multiplicando %rcx veces
            int id = etiquetas++;
            out << "movq %rax, %rdx" << endl;
            out << "movq $1, %rax" << endl;
            out << "pow_loop_" << id << ":" << endl;
            out << "cmpq $0, %rcx" << endl;
            out << "jle pow_end_" << id << endl;
            out << "imulq %rdx, %rax" << endl;
            out << "decq %rcx" << endl;
            out << "jmp pow_loop_" << id << endl;
            out << "pow_end_" << id << ":" << endl;
            break;
        }
        case LE_OP:
        case LT_OP:
        case GT_OP:
        case GE_OP:
        case EQ_OP:
        case NE_OP:
            // Compara y deja 1 (true) o 0 (false) en %rax
            out << "cmpq %rcx, %rax" << endl;
            out << instruccionSet(exp->op) << " %al" << endl;
            out << "movzbq %al, %rax" << endl;
            break;
        default:
            break;
    }
    if (aritmeticaPunteros) return l;              // p + i y p - i tienen el tipo de p
    // Valor de cada operando visto como entero (un bool vale 1 o 0)
    int a = l.isBool() ? l.boolValue : l.intValue;
    int b = r.isBool() ? r.boolValue : r.intValue;
    switch (exp->op) {
        case PLUS_OP:  return Type(a + b);
        case MINUS_OP: return Type(a - b);
        case MUL_OP:   return Type(a * b);
        case DIV_OP:   return Type(b != 0 ? a / b : 0);
        case POW_OP: {
            int p = 1;
            for (int i = 0; i < b; i++) p *= a;
            return Type(p);
        }
        case LE_OP:    return Type(a <= b);
        case LT_OP:    return Type(a < b);
        case GT_OP:    return Type(a > b);
        case GE_OP:    return Type(a >= b);
        case EQ_OP:    return Type(a == b);
        case NE_OP:    return Type(a != b);
        default:       return Type();
    }
}

// Carga el literal en %rax
Type GencodeVisitor::visit(NumberExp* exp) {
    out << "movq $" << exp->value << ", %rax" << endl;
    if (exp->esBool) return Type(exp->value != 0);   // true / false
    return Type(exp->value);
}

// Carga el valor de la variable desde la pila en %rax.
// Un struct no cabe en %rax: su valor es la dirección de su primer campo
Type GencodeVisitor::visit(IdExp* exp) {
    Type t = tipos.lookup(exp->value);
    string instr = t.isStruct() ? "leaq " : "movq ";
    out << instr << posicion.lookup(exp->value) * -TAM_VARIABLE << "(%rbp), %rax" << endl;
    return t;
}

// Genera el código del ifexp: deja en %rax el valor de la rama elegida
Type GencodeVisitor::visit(IfExp* exp) {
    int id = etiquetas++;
    Type c = exp->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je ifexp_else_" << id << endl;
    Type t = exp->thenExp->accept(this);
    out << "jmp ifexp_end_" << id << endl;
    out << "ifexp_else_" << id << ":" << endl;
    Type e = exp->elseExp->accept(this);
    out << "ifexp_end_" << id << ":" << endl;
    bool cond = c.isBool() ? c.boolValue : c.intValue != 0;
    return cond ? t : e;
}

// Llamada: evalúa los argumentos, los pasa en registros y llama a la función.
// El resultado queda en %rax.
Type GencodeVisitor::visit(FCallExp* exp) {
    for (Exp* arg : exp->argumentos) {
        arg->accept(this);
        out << "pushq %rax" << endl;
        pila++;
    }
    for (int i = (int)exp->argumentos.size() - 1; i >= 0; i--) {
        out << "popq " << REG_ARGS[i] << endl;
        pila--;
    }
    llamar(exp->nombre);
    return tipoDe(funciones[exp->nombre]->tipo);
}

// a[i]: calcula la dirección del elemento y carga su valor en %rax
// (si el elemento es un struct, se queda con su dirección)
Type GencodeVisitor::visit(IndexExp* exp) {
    Type t = direccion(exp);
    if (!t.isStruct()) out << "movq (%rax), %rax" << endl;
    return t;
}

// *p: evalúa el puntero y carga el valor guardado en esa dirección
Type GencodeVisitor::visit(DerefExp* exp) {
    Type t = direccion(exp);
    if (!t.isStruct()) out << "movq (%rax), %rax" << endl;
    return t;
}

// s.campo: calcula la dirección del campo y carga su valor en %rax
// (si el campo es un struct anidado, se queda con su dirección)
Type GencodeVisitor::visit(FieldAccessExp* exp) {
    Type t = direccion(exp);
    if (!t.isStruct()) out << "movq (%rax), %rax" << endl;
    return t;
}

// sizeof(tipo): el tamaño en bytes ya se conoce al compilar
Type GencodeVisitor::visit(SizeofExp* exp) {
    int bytes = tamTipo(tipoDe(exp->tipoNombre));
    out << "movq $" << bytes << ", %rax" << endl;
    return Type(bytes);
}

// Bytes que ocupa un valor del tipo: 8, o 8 por cada slot si es un struct
int GencodeVisitor::tamTipo(const Type& t) {
    return checker->slotsDe(t) * TAM_VARIABLE;
}

// &x: deja en %rax la dirección del lvalue
Type GencodeVisitor::visit(AddrExp* exp) {
    return direccion(exp->lvalue).direccion();
}

// malloc(bytes): pide memoria al heap; la dirección reservada vuelve en %rax
Type GencodeVisitor::visit(MallocExp* exp) {
    exp->bytes->accept(this);
    out << "movq %rax, %rdi" << endl;
    llamar("malloc@PLT");
    return Type::puntero(Type::VOID, 1);
}

// Métodos de pila. %rax tiene la dirección de la cabecera [tope, tamaño];
// cada nodo es [valor, siguiente]
Type GencodeVisitor::visit(StackOpExp* exp) {
    Type p = exp->pila->accept(this);
    switch (exp->op) {
        case PUSH_OP:
            // nodo = malloc(16); nodo->valor = e; nodo->siguiente = tope; tope = nodo; tamaño++
            out << "pushq %rax" << endl;
            pila++;
            exp->valor->accept(this);
            out << "pushq %rax" << endl;
            pila++;
            out << "movq $" << TAM_NODO_PILA << ", %rdi" << endl;
            llamar("malloc@PLT");
            out << "popq %rcx" << endl;            // valor
            out << "popq %rdx" << endl;            // cabecera
            pila -= 2;
            out << "movq %rcx, (%rax)" << endl;
            out << "movq (%rdx), %rcx" << endl;
            out << "movq %rcx, 8(%rax)" << endl;
            out << "movq %rax, (%rdx)" << endl;
            out << "incq 8(%rdx)" << endl;
            return Type();
        case POP_OP:
            // nodo = tope (error si es nulo); tope = nodo->siguiente; tamaño--; free(nodo)
            usaPilaVacia = true;
            out << "movq (%rax), %rdi" << endl;
            out << "cmpq $0, %rdi" << endl;
            out << "je .Lpila_vacia" << endl;
            out << "movq 8(%rdi), %rcx" << endl;
            out << "movq %rcx, (%rax)" << endl;
            out << "decq 8(%rax)" << endl;
            out << "pushq (%rdi)" << endl;         // guarda el valor mientras se libera el nodo
            pila++;
            llamar("free@PLT");
            out << "popq %rax" << endl;
            pila--;
            return p.elemento();
        case TOP_OP:
            usaPilaVacia = true;
            out << "movq (%rax), %rax" << endl;
            out << "cmpq $0, %rax" << endl;
            out << "je .Lpila_vacia" << endl;
            out << "movq (%rax), %rax" << endl;
            return p.elemento();
        case EMPTY_OP:
            out << "cmpq $0, 8(%rax)" << endl;
            out << "sete %al" << endl;
            out << "movzbq %al, %rax" << endl;
            return Type(false);
        case SIZE_OP:
            out << "movq 8(%rax), %rax" << endl;
            return Type(0);
        default:
            return Type();
    }
}

// Deja en %rax la dirección de un lvalue y devuelve el tipo del valor guardado ahí:
//   x       -> posición de x en la pila
//   a[i]    -> a + i * tamaño del elemento (a es la dirección del primer elemento)
//   *p      -> el valor de p
//   s.campo -> dirección de s (o valor de s si es un puntero a struct) + offset del campo
Type GencodeVisitor::direccion(Exp* lvalue) {
    if (IdExp* id = dynamic_cast<IdExp*>(lvalue)) {
        out << "leaq " << posicion.lookup(id->value) * -TAM_VARIABLE << "(%rbp), %rax" << endl;
        return tipos.lookup(id->value);
    }
    if (IndexExp* acceso = dynamic_cast<IndexExp*>(lvalue)) {
        Type base = acceso->base->accept(this);
        out << "pushq %rax" << endl;
        pila++;
        acceso->indice->accept(this);
        out << "movq %rax, %rcx" << endl;
        out << "popq %rax" << endl;
        pila--;
        Type elemento = base.apuntado();
        int tam = tamTipo(elemento);
        if (tam == TAM_VARIABLE) {
            out << "leaq (%rax,%rcx," << TAM_VARIABLE << "), %rax" << endl;
        }
        else {
            out << "imulq $" << tam << ", %rcx" << endl;
            out << "addq %rcx, %rax" << endl;
        }
        return elemento;
    }
    if (DerefExp* deref = dynamic_cast<DerefExp*>(lvalue)) {
        return deref->puntero->accept(this).apuntado();
    }
    if (FieldAccessExp* acceso = dynamic_cast<FieldAccessExp*>(lvalue)) {
        // Tanto un struct como un puntero a struct dejan en %rax la dirección del struct
        Type obj = acceso->objeto->accept(this);
        const CampoInfo* c = checker->campo(obj.structName, acceso->campo);
        if (c->offset != 0) out << "addq $" << c->offset << ", %rax" << endl;
        return tipoDe(c->tipo);
    }
    throw runtime_error("la expresión no tiene dirección");
}

// Llama a la función con la pila alineada a 16 bytes, como exige System V
void GencodeVisitor::llamar(const string& funcion) {
    bool alinear = pila % 2 != 0;
    if (alinear) out << "subq $8, %rsp" << endl;
    out << "call " << funcion << endl;
    if (alinear) out << "addq $8, %rsp" << endl;
}

// Evalúa la expresión y la guarda en la posición de la variable
void GencodeVisitor::visit(AssignStmt* stm) {
    Type t = stm->e->accept(this);
    if (!t.isPtr()) tipos.update(stm->id, t);      // un puntero conserva su tipo declarado (malloc da void*)
    out << "movq %rax, " << posicion.lookup(stm->id) * -TAM_VARIABLE << "(%rbp)" << endl;
}

// s.push(e) | s.pop(): genera el método; el valor de pop se descarta
void GencodeVisitor::visit(StackStmt* stm) {
    stm->op->accept(this);
}

// a[i] = e  |  *p = e: calcula la dirección destino, evalúa e y la guarda en memoria
void GencodeVisitor::visit(StoreStmt* stm) {
    direccion(stm->destino);
    out << "pushq %rax" << endl;
    pila++;
    stm->e->accept(this);
    out << "popq %rcx" << endl;
    pila--;
    out << "movq %rax, (%rcx)" << endl;
}

// free(p): devuelve al heap la memoria reservada con malloc
void GencodeVisitor::visit(FreeStmt* stm) {
    stm->e->accept(this);
    out << "movq %rax, %rdi" << endl;
    llamar("free@PLT");
}

// Evalúa la expresión y la imprime con printf
void GencodeVisitor::visit(PrintStmt* stm) {
    stm->e->accept(this);
    out << "movq %rax, %rsi" << endl;
    out << "leaq print_fmt(%rip), %rdi" << endl;
    out << "movl $0, %eax" << endl;
    out << "call printf@PLT" << endl;
}

// Genera el código del if: si la condición es 0 salta a la rama else
void GencodeVisitor::visit(IfStmt* stm) {
    int id = etiquetas++;
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je else_" << id << endl;
    for (Stmt* s : stm->thenList) s->accept(this);
    out << "jmp endif_" << id << endl;
    out << "else_" << id << ":" << endl;
    for (Stmt* s : stm->elseList) s->accept(this);
    out << "endif_" << id << ":" << endl;
}

// Genera el código del while: evalúa la condición al inicio de cada vuelta
void GencodeVisitor::visit(WhileStmt* stm) {
    int id = etiquetas++;
    string fin = "endwhile_" + to_string(id);
    out << "while_" << id << ":" << endl;
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je " << fin << endl;
    finDeBucle.push_back(fin);
    for (Stmt* s : stm->body) s->accept(this);
    finDeBucle.pop_back();
    out << "jmp while_" << id << endl;
    out << fin << ":" << endl;
}

// Genera el código del do-while: ejecuta el cuerpo y repite mientras la condición no sea 0
void GencodeVisitor::visit(DoWhileStmt* stm) {
    int id = etiquetas++;
    string fin = "enddowhile_" + to_string(id);
    out << "dowhile_" << id << ":" << endl;
    finDeBucle.push_back(fin);
    for (Stmt* s : stm->body) s->accept(this);
    finDeBucle.pop_back();
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "jne dowhile_" << id << endl;
    out << fin << ":" << endl;
}

// Genera el código del break: salta a la salida del bucle más interno
void GencodeVisitor::visit(BreakStmt* stm) {
    out << "jmp " << finDeBucle.back() << endl;
}

// Genera el código del return: deja el valor en %rax y salta al epílogo de la función
void GencodeVisitor::visit(ReturnStmt* stm) {
    stm->exp->accept(this);
    out << "jmp .Lfin_" << funcionActual << endl;
}

// Declara cada variable en el nivel actual: le asigna una posición en la pila y su tipo.
// Una pila nace vacía: se reserva su cabecera en el heap con tope = 0 y tamaño = 0.
// Un struct ocupa N slots seguidos; su posición es la del slot más bajo, para que
// el campo con offset k quede en dirección + k (direcciones crecientes)
void GencodeVisitor::visit(VarDec* dec) {
    Type tipo = tipoDe(dec->tipo);
    for (const string& v : dec->variables) {
        int slots = checker->slotsDe(tipo);
        contador += slots - 1;
        posicion.add_var(v, contador++);
        tipos.add_var(v, tipo);
        int offset = posicion.lookup(v) * -TAM_VARIABLE;
        if (tipo.isStruct()) {
            for (int i = 0; i < slots; i++) {
                out << "movq $0, " << offset + i * TAM_VARIABLE << "(%rbp)" << endl;   // campos en 0
            }
        }
        else if (tipo.isStack()) {
            out << "movq $" << TAM_CABECERA_PILA << ", %rdi" << endl;
            llamar("malloc@PLT");
            out << "movq $0, (%rax)" << endl;
            out << "movq $0, 8(%rax)" << endl;
            out << "movq %rax, " << offset << "(%rbp)" << endl;
        }
        else {
            out << "movq $0, " << offset << "(%rbp)" << endl;   // inicia en 0 / false
        }
    }
}

// Procesa primero las declaraciones y luego las sentencias
void GencodeVisitor::visit(Body* body) {
    for (VarDec* d : body->declaraciones) d->accept(this);
    for (Stmt* s : body->sentencias) s->accept(this);
}

// Genera una función: prólogo, copia de parámetros a la pila, cuerpo y epílogo
void GencodeVisitor::visit(FunDec* fun) {
    funcionActual = fun->nombre;
    contador = 1;
    posicion.add_level();
    tipos.add_level();

    // Espacio en la pila según lo que contó el TypeChecker
    int espacio = checker->espacioPila(fun->nombre);

    out << fun->nombre << ":" << endl;
    out << "pushq %rbp" << endl;
    out << "movq %rsp, %rbp" << endl;
    if (espacio > 0) out << "subq $" << espacio << ", %rsp" << endl;

    // Los parámetros llegan en registros: se guardan en la pila como variables locales
    for (size_t i = 0; i < fun->parametros_id.size(); i++) {
        const string& p = fun->parametros_id[i];
        posicion.add_var(p, contador++);
        tipos.add_var(p, tipoDe(fun->parametros_tipos[i]));
        out << "movq " << REG_ARGS[i] << ", " << posicion.lookup(p) * -TAM_VARIABLE << "(%rbp)" << endl;
    }

    fun->cuerpo->accept(this);

    out << "movq $0, %rax" << endl;            // si no hubo return, devuelve 0
    out << ".Lfin_" << fun->nombre << ":" << endl;
    if (fun->nombre == "main") {
        out << "movq $0, %rax" << endl;        // main siempre termina con código 0
    }
    out << "leave" << endl;
    out << "ret" << endl;

    posicion.remove_level();
    tipos.remove_level();
}

// Genera el programa completo: sección de datos y el código de cada función
void GencodeVisitor::visit(Programa* program) {
    for (FunDec* f : program->funciones) funciones[f->nombre] = f;

    out << ".data" << endl;
    out << "print_fmt:.string \"%ld\\n\" " << endl;
    out << ".text" << endl;
    out << ".globl main" << endl;
    for (FunDec* f : program->funciones) f->accept(this);
    if (usaPilaVacia) {
        // pop / top sobre una pila vacía: mensaje en stderr y exit(1)
        out << ".Lpila_vacia:" << endl;
        out << "andq $-16, %rsp" << endl;
        out << "movq stderr@GOTPCREL(%rip), %rax" << endl;
        out << "movq (%rax), %rdi" << endl;
        out << "leaq pila_vacia_msg(%rip), %rsi" << endl;
        out << "movl $0, %eax" << endl;
        out << "call fprintf@PLT" << endl;
        out << "movl $1, %edi" << endl;
        out << "call exit@PLT" << endl;
        out << ".data" << endl;
        out << "pila_vacia_msg:.string \"Error: pop/top sobre una pila vacia\\n\"" << endl;
    }
    out << ".section .note.GNU-stack,\"\",@progbits" << endl;
}

// =====================================================
// ==================== TypeChecker ====================
// =====================================================

// Lanza un error de tipos con el mensaje dado
static void error(const string& msg) {
    throw runtime_error(msg);
}

// Verifica todo el programa
void TypeChecker::check(Programa* program) {
    program->accept(this);
}

// Bytes que debe reservar la función en la pila: 8 por variable (int o bool),
// redondeado a múltiplo de 16 para mantener la pila alineada
int TypeChecker::espacioPila(const string& funcion) {
    int bytes = numVariables[funcion] * TAM_VARIABLE;
    return ((bytes + 15) / 16) * 16;
}

// Slots de 8 bytes que ocupa un valor del tipo: los structs, la suma de sus campos; el resto, 1
int TypeChecker::slotsDe(const Type& t) const {
    if (t.isStruct()) return structs.at(t.structName).tamSlots;
    return 1;
}

// Información del campo de un struct, o nullptr si el struct no tiene ese campo
const CampoInfo* TypeChecker::campo(const string& structNombre, const string& nombre) const {
    auto s = structs.find(structNombre);
    if (s == structs.end()) return nullptr;
    for (const auto& c : s->second.campos) {
        if (c.first == nombre) return &c.second;
    }
    return nullptr;
}

// Verifica que el tipo exista: int, bool, stack<...> o un struct declarado (con o sin '*')
void TypeChecker::validarTipo(const string& nombre, const string& donde) {
    if (nombre.rfind("stack<", 0) == 0) {
        string elemento = nombre.substr(6, nombre.size() - 7);
        string base = elemento.substr(0, elemento.find('*'));
        if (base != "int" && base != "bool") {
            error("tipo desconocido en " + donde + ": " + nombre + " (una pila guarda int, bool o punteros)");
        }
        return;
    }
    string base = nombre.substr(0, nombre.find('*'));
    if (base != "int" && base != "bool" && !structs.count(base)) {
        error("tipo desconocido en " + donde + ": " + nombre);
    }
}

// Registra un struct: cada campo ocupa sus slots uno tras otro; un struct anidado va en línea.
// Un campo puede ser un puntero al propio struct (Nodo* sig), pero no el struct por valor
void TypeChecker::registrarStruct(StructDec* s) {
    if (structs.count(s->nombre)) {
        error("struct declarado dos veces: " + s->nombre);
    }
    StructInfo info;
    info.nombre = s->nombre;
    info.tamSlots = 0;
    structs[s->nombre] = info;                     // ya visible para punteros a sí mismo
    for (const StructField& f : s->campos) {
        if (f.tipo == s->nombre) {
            error("el struct " + s->nombre + " no puede contenerse a sí mismo (usa " + s->nombre + "*)");
        }
        validarTipo(f.tipo, "el struct " + s->nombre);
        if (campo(s->nombre, f.nombre)) {
            error("campo repetido en " + s->nombre + ": " + f.nombre);
        }
        Type t = tipoDe(f.tipo);
        if (t.isStack()) {
            error("el campo " + f.nombre + " de " + s->nombre + " no puede ser una pila");
        }
        structs[s->nombre].campos.push_back({f.nombre, {f.tipo, structs[s->nombre].tamSlots * TAM_VARIABLE}});
        structs[s->nombre].tamSlots += slotsDe(t);
    }
    if (structs[s->nombre].tamSlots == 0) {
        error("el struct " + s->nombre + " no tiene campos");
    }
}

// Verifica que la condición de una sentencia sea bool
void TypeChecker::esperarBool(Exp* condicion, const string& donde) {
    Type c = condicion->accept(this);
    if (!c.isBool()) {
        error("la condición del " + donde + " debe ser bool, pero es " + c.kindName());
    }
}

// ---------- Expresiones ----------

// Operadores aritméticos: int op int -> int
// Orden: int (<= | < | > | >=) int -> bool
// Igualdad: a (== | !=) b con a y b del mismo tipo (int, bool o puntero) -> bool
// Aritmética de punteros: puntero + int y puntero - int -> puntero del mismo tipo
Type TypeChecker::visit(BinaryExp* exp) {
    Type l = exp->left->accept(this);
    Type r = exp->right->accept(this);
    string op = Exp::binopToChar(exp->op);
    if (exp->op == EQ_OP || exp->op == NE_OP) {
        if ((l.isPtr() && esNulo(exp->right)) || (r.isPtr() && esNulo(exp->left))) {
            return Type(false);                  // p == 0, p != 0
        }
        if (l.isVoid() || l.isStruct() || r.isStruct() || (!l.aceptaValor(r) && !r.aceptaValor(l))) {
            error("el operador '" + op + "' necesita dos operandos del mismo tipo, pero recibió "
                  + l.kindName() + " " + op + " " + r.kindName());
        }
        return Type(false);                      // bool
    }
    if (l.isPtr() && r.isInt() && (exp->op == PLUS_OP || exp->op == MINUS_OP)) {
        return l;
    }
    if (!l.isInt() || !r.isInt()) {
        error("el operador '" + op + "' necesita operandos int, pero recibió "
              + l.kindName() + " " + op + " " + r.kindName());
    }
    switch (exp->op) {
        case LE_OP:
        case LT_OP:
        case GT_OP:
        case GE_OP:
            return Type(false);                  // bool
        default:
            return Type(0);                      // int
    }
}

// Un número es int; true / false son bool
Type TypeChecker::visit(NumberExp* exp) {
    if (exp->esBool) return Type(false);
    return Type(0);
}

// Una variable tiene el tipo con el que fue declarada
Type TypeChecker::visit(IdExp* exp) {
    if (!tipos.check(exp->value)) {
        error("variable no declarada: " + exp->value);
    }
    return tipos.lookup(exp->value);
}

// ifexp(c, e1, e2): c debe ser bool y ambas ramas del mismo tipo
Type TypeChecker::visit(IfExp* exp) {
    Type c = exp->condicion->accept(this);
    if (!c.isBool()) {
        error("la condición de ifexp debe ser bool, pero es " + c.kindName());
    }
    Type t = exp->thenExp->accept(this);
    Type e = exp->elseExp->accept(this);
    if (t.aceptaValor(e)) return t;
    if (e.aceptaValor(t)) return e;              // p. ej. ifexp(c, malloc(8), p) es del tipo de p
    error("las ramas de ifexp deben tener el mismo tipo, pero son "
          + t.kindName() + " y " + e.kindName());
    return t;
}

// f(a1, ..., an): la función existe, recibe n argumentos y cada uno tiene el tipo de su parámetro
Type TypeChecker::visit(FCallExp* exp) {
    if (!funciones.count(exp->nombre)) {
        error("función no declarada: " + exp->nombre);
    }
    FunDec* f = funciones[exp->nombre];
    if (exp->argumentos.size() != f->parametros_id.size()) {
        error("la función " + f->nombre + " espera " + to_string(f->parametros_id.size())
              + " argumento(s), pero recibió " + to_string(exp->argumentos.size()));
    }
    for (size_t i = 0; i < exp->argumentos.size(); i++) {
        Type t = exp->argumentos[i]->accept(this);
        Type param = tipoDe(f->parametros_tipos[i]);
        if (!param.aceptaValor(t) && !(param.isPtr() && esNulo(exp->argumentos[i]))) {
            error("el argumento " + to_string(i + 1) + " de " + f->nombre + " debe ser "
                  + f->parametros_tipos[i] + ", pero es " + t.kindName());
        }
    }
    return tipoDe(f->tipo);
}

// a[i]: a es un puntero (int*, int**, ...) e i un int; el resultado es el tipo apuntado
Type TypeChecker::visit(IndexExp* exp) {
    Type b = exp->base->accept(this);
    Type i = exp->indice->accept(this);
    if (!b.isPtr()) {
        error("solo se puede indexar un puntero, pero se indexó un " + b.kindName());
    }
    if (b.isVoidPtr()) {
        error("no se puede indexar un void*: guarda el malloc en una variable con tipo");
    }
    if (!i.isInt()) {
        error("el índice de un arreglo debe ser int, pero es " + i.kindName());
    }
    return b.apuntado();
}

// *p: p es un puntero; el resultado es el tipo apuntado
Type TypeChecker::visit(DerefExp* exp) {
    Type p = exp->puntero->accept(this);
    if (!p.isPtr()) {
        error("solo se puede desreferenciar (*) un puntero, pero se recibió " + p.kindName());
    }
    if (p.isVoidPtr()) {
        error("no se puede desreferenciar un void*");
    }
    return p.apuntado();
}

// &x: x debe tener dirección (variable, a[i], *p o s.campo); el resultado es un puntero a su tipo
Type TypeChecker::visit(AddrExp* exp) {
    if (!esLvalue(exp->lvalue)) {
        error("'&' necesita una variable, un elemento a[i], un *p o un s.campo");
    }
    Type t = exp->lvalue->accept(this);
    if (t.isStack()) {
        error("no se puede tomar la dirección (&) de una pila");
    }
    return t.direccion();
}

// malloc(n): n es la cantidad de bytes (int); devuelve un void* asignable a cualquier puntero
Type TypeChecker::visit(MallocExp* exp) {
    Type b = exp->bytes->accept(this);
    if (!b.isInt()) {
        error("malloc necesita la cantidad de bytes (int), pero recibió " + b.kindName());
    }
    return Type::puntero(Type::VOID, 1);
}

// s.campo: s es un struct o un puntero a struct (se desreferencia solo, como p->campo en C);
// el resultado es el tipo del campo
Type TypeChecker::visit(FieldAccessExp* exp) {
    Type obj = exp->objeto->accept(this);
    bool esStruct = obj.isStruct() || (obj.isStructPtr() && obj.nivel == 1);
    if (!esStruct) {
        error("." + exp->campo + " solo se puede usar sobre un struct o un puntero a struct, pero se usó sobre un "
              + obj.kindName());
    }
    const CampoInfo* c = campo(obj.structName, exp->campo);
    if (!c) {
        error("el struct " + obj.structName + " no tiene el campo " + exp->campo);
    }
    return tipoDe(c->tipo);
}

// sizeof(tipo): el tipo debe existir; el resultado es un int
Type TypeChecker::visit(SizeofExp* exp) {
    validarTipo(exp->tipoNombre, "sizeof");
    return Type(slotsDe(tipoDe(exp->tipoNombre)) * TAM_VARIABLE);
}

// s.metodo(...): s debe ser una pila.
//   push(e): e del tipo de los elementos; no devuelve valor
//   pop() / top(): devuelven un elemento    empty(): bool    size(): int
Type TypeChecker::visit(StackOpExp* exp) {
    Type p = exp->pila->accept(this);
    if (!p.isStack()) {
        error("." + exp->metodo() + "() solo se puede usar sobre una pila, pero se usó sobre un " + p.kindName());
    }
    switch (exp->op) {
        case PUSH_OP: {
            Type v = exp->valor->accept(this);
            if (!p.elemento().aceptaValor(v)) {
                error("no se puede apilar " + v.kindName() + " en una " + p.kindName());
            }
            return Type();                       // void: push no se puede usar como valor
        }
        case POP_OP:
        case TOP_OP:
            return p.elemento();
        case EMPTY_OP:
            return Type(false);                  // bool
        default:
            return Type(0);                      // size: int
    }
}

// ---------- Sentencias ----------

// x = e: x declarada y e del mismo tipo
void TypeChecker::visit(AssignStmt* stm) {
    if (!tipos.check(stm->id)) {
        error("variable no declarada: " + stm->id);
    }
    Type declarado = tipos.lookup(stm->id);
    Type t = stm->e->accept(this);
    if (declarado.isStruct()) {
        error("no se puede asignar un struct completo (" + stm->id + "); asigna campo por campo");
    }
    if (!declarado.aceptaValor(t) && !(declarado.isPtr() && esNulo(stm->e))) {
        error("no se puede asignar " + t.kindName() + " a la variable "
              + declarado.kindName() + " " + stm->id);
    }
}

// s.push(e) | s.pop(): se verifica el método
void TypeChecker::visit(StackStmt* stm) {
    stm->op->accept(this);
}

// a[i] = e  |  *p = e: el valor debe tener el tipo del elemento
void TypeChecker::visit(StoreStmt* stm) {
    Type destino = stm->destino->accept(this);
    Type t = stm->e->accept(this);
    if (destino.isStruct()) {
        error("no se puede asignar un struct completo (" + destino.kindName() + "); asigna campo por campo");
    }
    if (!destino.aceptaValor(t) && !(destino.isPtr() && esNulo(stm->e))) {
        error("no se puede asignar " + t.kindName() + " a un elemento de tipo " + destino.kindName());
    }
}

// free(p): p debe ser un puntero
void TypeChecker::visit(FreeStmt* stm) {
    Type t = stm->e->accept(this);
    if (!t.isPtr()) {
        error("free necesita un puntero, pero recibió " + t.kindName());
    }
}

// print(e): e debe ser int o bool (una dirección no se imprime)
void TypeChecker::visit(PrintStmt* stm) {
    Type t = stm->e->accept(this);
    if (!t.isInt() && !t.isBool()) {
        error("print necesita una expresión int o bool, pero recibió " + t.kindName());
    }
}

// if: condición bool y se verifican ambas ramas
void TypeChecker::visit(IfStmt* stm) {
    esperarBool(stm->condicion, "if");
    for (Stmt* s : stm->thenList) s->accept(this);
    for (Stmt* s : stm->elseList) s->accept(this);
}

// while: condición bool y se verifica el cuerpo
void TypeChecker::visit(WhileStmt* stm) {
    esperarBool(stm->condicion, "while");
    bucles++;
    for (Stmt* s : stm->body) s->accept(this);
    bucles--;
}

// do-while: se verifica el cuerpo y la condición debe ser bool
void TypeChecker::visit(DoWhileStmt* stm) {
    bucles++;
    for (Stmt* s : stm->body) s->accept(this);
    bucles--;
    esperarBool(stm->condicion, "do-while");
}

// break: solo dentro de un bucle
void TypeChecker::visit(BreakStmt* stm) {
    if (bucles == 0) {
        error("'break' fuera de un bucle");
    }
}

// return e: e debe tener el tipo de la función
void TypeChecker::visit(ReturnStmt* stm) {
    Type t = stm->exp->accept(this);
    Type esperado = tipoDe(funcionActual->tipo);
    if (!esperado.aceptaValor(t) && !(esperado.isPtr() && esNulo(stm->exp))) {
        error("la función " + funcionActual->nombre + " debe devolver "
              + funcionActual->tipo + ", pero return devuelve " + t.kindName());
    }
}

// ---------- Declaraciones y programa ----------

// var tipo ids: tipo válido y sin variables repetidas
void TypeChecker::visit(VarDec* dec) {
    validarTipo(dec->tipo, "la declaración de variables");
    Type t = tipoDe(dec->tipo);
    for (const string& v : dec->variables) {
        if (tipos.check(v)) {
            error("variable declarada dos veces: " + v);
        }
        tipos.add_var(v, t);
        numVariables[funcionActual->nombre] += slotsDe(t);   // un struct ocupa varios slots
    }
}

// Primero las declaraciones, luego las sentencias
void TypeChecker::visit(Body* body) {
    for (VarDec* d : body->declaraciones) d->accept(this);
    for (Stmt* s : body->sentencias) s->accept(this);
}

// Una función abre su propio nivel con sus parámetros y variables locales
void TypeChecker::visit(FunDec* fun) {
    funcionActual = fun;
    numFunciones++;
    numVariables[fun->nombre] = fun->parametros_id.size();   // los parámetros también ocupan pila
    tipos.add_level();
    for (size_t i = 0; i < fun->parametros_id.size(); i++) {
        const string& p = fun->parametros_id[i];
        if (tipos.check(p)) {
            error("parámetro repetido en " + fun->nombre + ": " + p);
        }
        tipos.add_var(p, tipoDe(fun->parametros_tipos[i]));
    }
    fun->cuerpo->accept(this);
    tipos.remove_level();
    funcionActual = nullptr;
}

// Registra los structs (en orden: un struct solo puede contener por valor a otro declarado antes),
// luego todas las funciones (para permitir recursión y llamadas a funciones
// definidas más abajo) y por último verifica cada una
void TypeChecker::visit(Programa* program) {
    for (StructDec* s : program->estructuras) registrarStruct(s);
    for (FunDec* f : program->funciones) {
        validarTipo(f->tipo, "el retorno de " + f->nombre);
        if (tipoDe(f->tipo).isStruct()) {
            error("la función " + f->nombre + " no puede devolver un struct por valor (usa " + f->tipo + "*)");
        }
        for (size_t i = 0; i < f->parametros_tipos.size(); i++) {
            validarTipo(f->parametros_tipos[i], "los parámetros de " + f->nombre);
            if (tipoDe(f->parametros_tipos[i]).isStruct()) {
                error("el parámetro " + f->parametros_id[i] + " de " + f->nombre
                      + " no puede ser un struct por valor (usa " + f->parametros_tipos[i] + "*)");
            }
        }
        if (funciones.count(f->nombre)) {
            error("función declarada dos veces: " + f->nombre);
        }
        if (f->parametros_id.size() > 6) {
            error("la función " + f->nombre + " tiene más de 6 parámetros");
        }
        funciones[f->nombre] = f;
    }
    if (!funciones.count("main")) {
        error("falta la función main");
    }
    FunDec* m = funciones["main"];
    if (m->tipo != "int" || !m->parametros_id.empty()) {
        error("main debe declararse como: fun int main()");
    }
    for (FunDec* f : program->funciones) f->accept(this);
}
