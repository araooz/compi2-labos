#include "type.h"

// Tipo vacío, lo devuelven las sentencias
Type::Type() : kind(VOID), intValue(0), boolValue(false), base(VOID), nivel(0) {}

// Tipo entero: el valor se guarda en intValue
Type::Type(int v) : kind(INT), intValue(v), boolValue(false), base(VOID), nivel(0) {}

// Tipo booleano: el valor se guarda en boolValue
Type::Type(bool b) : kind(BOOL), intValue(0), boolValue(b), base(VOID), nivel(0) {}

// Tipo puntero con su tipo base y su cantidad de '*'.
// Con nivel 0 no es un puntero: devuelve directamente el tipo base.
Type Type::puntero(Kind base, int nivel) {
    if (nivel <= 0) return base == BOOL ? Type(false) : base == INT ? Type(0) : Type();
    Type t;
    t.kind = PTR;
    t.base = base;
    t.nivel = nivel;
    return t;
}

// Tipo pila cuyos elementos son del tipo dado (int, bool o un puntero)
Type Type::pila(const Type& elemento) {
    Type t;
    t.kind = STACK;
    t.base = elemento.isPtr() ? elemento.base : elemento.kind;
    t.nivel = elemento.isPtr() ? elemento.nivel : 0;
    return t;
}

// Tipo que corresponde a un nombre: int, bool, int*, bool**, stack<int>, ...
Type Type::desdeNombre(const string& nombre) {
    if (nombre.rfind("stack<", 0) == 0) {
        return pila(desdeNombre(nombre.substr(6, nombre.size() - 7)));   // lo que va entre < y >
    }
    size_t estrella = nombre.find('*');
    string baseNombre = nombre.substr(0, estrella);
    int nivel = estrella == string::npos ? 0 : (int)(nombre.size() - estrella);
    return puntero(baseNombre == "bool" ? BOOL : INT, nivel);
}

bool Type::isInt() const     { return kind == INT; }
bool Type::isBool() const    { return kind == BOOL; }
bool Type::isVoid() const    { return kind == VOID; }
bool Type::isPtr() const     { return kind == PTR; }
bool Type::isVoidPtr() const { return kind == PTR && base == VOID; }
bool Type::isStack() const   { return kind == STACK; }

// Tipo de los elementos de la pila
Type Type::elemento() const {
    return puntero(base, nivel);
}

// Tipo del valor apuntado: quita un '*'
Type Type::apuntado() const {
    return puntero(base, nivel - 1);
}

// Tipo de la dirección de un valor de este tipo: agrega un '*'
Type Type::direccion() const {
    if (isPtr()) return puntero(base, nivel + 1);
    return puntero(kind, 1);
}

// Dos tipos son iguales si coinciden en clase y, si son punteros o pilas, en base y nivel
bool Type::mismoTipo(const Type& otro) const {
    if (kind != otro.kind) return false;
    if (kind != PTR && kind != STACK) return true;
    return base == otro.base && nivel == otro.nivel;
}

// Un valor se puede asignar si tiene el mismo tipo; además, el void* de malloc
// se puede asignar a cualquier puntero (como en C)
bool Type::aceptaValor(const Type& valor) const {
    if (isPtr() && valor.isVoidPtr()) return true;
    return mismoTipo(valor);
}

// Nombre del tipo, útil para mensajes de error o depuración
string Type::kindName() const {
    switch (kind) {
        case INT:  return "int";
        case BOOL: return "bool";
        case PTR:  return (base == INT ? "int" : base == BOOL ? "bool" : "void") + string(nivel, '*');
        case STACK: return "stack<" + elemento().kindName() + ">";
        default:   return "void";
    }
}

// Imprime el tipo con su valor, por ejemplo int(5), bool(true) o int**
ostream& operator<<(ostream& outs, const Type& t) {
    switch (t.kind) {
        case Type::INT:  return outs << "int(" << t.intValue << ")";
        case Type::BOOL: return outs << "bool(" << (t.boolValue ? "true" : "false") << ")";
        case Type::PTR:
        case Type::STACK: return outs << t.kindName();
        default:         return outs << "void";
    }
}
