#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:  return "+";
        case MINUS_OP: return "-";
        case MUL_OP:   return "*";
        case DIV_OP:   return "/";
        case POW_OP:   return "**";
        default:       return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o)
    : left(l), right(r), op(o) {}

    
BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}



// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}


// ------------------ FloatExp ------------------
FloatExp::FloatExp(double v) : value(v) {}

FloatExp::~FloatExp() {}


// ------------------idExp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}

// ------------------ SqrtExp ------------------
SqrtExp::SqrtExp(Exp* v) : value(v) {}

SqrtExp::~SqrtExp() { delete value; }

// ------------------ AbsExp ------------------
AbsExp::AbsExp(Exp* v) : value(v) {}

AbsExp::~AbsExp() { delete value; }

void BinaryExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\""
        << Exp::binopToChar(op) << "\"];\n";

    if (left) {
        int leftId = id;
        left->toDot(out, id);
        out << "  node" << myId << " -> node" << leftId << ";\n";
    }
    if (right) {
        int rightId = id;
        right->toDot(out, id);
        out << "  node" << myId << " -> node" << rightId << ";\n";
    }
}
// ----------------- MAX y MIN ----------------
MaxExp::MaxExp(list<Exp*> el) : exp_list(el) {}
MaxExp::~MaxExp() {
    for (Exp* exp : exp_list) {
        delete exp;
    }
}

MinExp::MinExp(list<Exp*> el) : exp_list(el) {}
MinExp::~MinExp() {
    for (Exp* exp : exp_list) {
        delete exp;
    }
}


void NumberExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\"" << value << "\"];\n";
}

void FloatExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\"" << value << "\"];\n";
}

void SqrtExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\"sqrt\"];\n";

    if (value) {
        int childId = id;
        value->toDot(out, id);
        out << "  node" << myId << " -> node" << childId << ";\n";
    }
}

void AbsExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\"abs\"];\n";

    if (value) {
        int childId = id;
        value->toDot(out, id);
        out << "  node" << myId << " -> node" << childId << ";\n";
    }
}

void MaxExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\"max\"];\n";
    for (Exp* exp : exp_list) {
        if (exp) {
            int childId = id;
            exp->toDot(out, id);
            out << "  node" << myId << " -> node" << childId << ";\n";
        }
    }
}

void MinExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\"min\"];\n";
    for (Exp* exp : exp_list) {
        if (exp) {
            int childId = id;
            exp->toDot(out, id);
            out << "  node" << myId << " -> node" << childId << ";\n";
        }
    }
}

void IdExp::toDot(ostream& out, int& id) const {
    int myId = id++;
    out << "  node" << myId << " [label=\"" << value << "\"];\n";
}
