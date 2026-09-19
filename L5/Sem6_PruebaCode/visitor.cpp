#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include "ast.h"
#include "visitor.h"


using namespace std;
unordered_map<std::string, int> memoria;

// Excepción interna para señalizar break dentro de switch
class BreakSignal : public exception {};

///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int SqrtExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Program::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int PrintStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Body::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IfStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ElifStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int DoWhileStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int WhileStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BreakStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int CaseStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int SwitchStatement::accept(Visitor* visitor) {
    return visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(SqrtExp* exp) {
    cout << "sqrt(";
    exp->value->accept(this);
    cout <<  ")";
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    cout << (exp->value ? "true" : "false");
    return 0;
}

int PrintVisitor::visit(Program* p) {
    cout << "PROGRAMA" << endl;
    for (auto i: p->cuerpo)
    {
        i->accept(this);
    }
    
    return 0;
}

int PrintVisitor::visit(PrintStatement* p) {
    cout << "print(" ;
    p->valor->accept(this);
    cout << ")" << endl;
    return 0;
}

int PrintVisitor::visit(AssignStatement* p) {
    cout << p->variable << "=";
    p->valor->accept(this);
    cout << endl; 
    return 0;
}


int PrintVisitor::visit(IdExp* p) {
    cout << p->value ;
    return 0;
}

int PrintVisitor::visit(Body* p) {
    for (auto stm : p->list_stm) {
        stm->accept(this);
    }
    return 0;
}

int PrintVisitor::visit(IfStatement* p) {
    cout << "if ";
    p->condition->accept(this);
    cout << " then " << endl;
    p->ifbody->accept(this);
    for (auto elif : p->elif_list) {
        elif->accept(this);
    }
    if (p->elsebody) {
        cout << "else" << endl;
        p->elsebody->accept(this);
    }
    cout << "endif" << endl;
    return 0;
}

int PrintVisitor::visit(ElifStatement* p) {
    cout << "elif ";
    p->condition->accept(this);
    cout << " then " << endl;
    p->elifbody->accept(this);
    return 0;
}

int PrintVisitor::visit(DoWhileStatement* p) {
    cout << "do" << endl;
    p->body->accept(this);
    cout << "while ";
    p->condition->accept(this);
    cout << endl;
    return 0;
}

int PrintVisitor::visit(WhileStatement* p) {
    cout << "while ";
    p->condition->accept(this);
    cout << " do" << endl;
    p->body->accept(this);
    cout << "endwhile" << endl;
    return 0;
}

int PrintVisitor::visit(BreakStatement* p) {
    cout << "break" << endl;
    return 0;
}

int PrintVisitor::visit(CaseStatement* p) {
    cout << "case ";
    p->value->accept(this);
    cout << endl;
    p->body->accept(this);
    if (p->hasBreak) {
        cout << "break" << endl;
    }
    return 0;
}

int PrintVisitor::visit(SwitchStatement* p) {
    cout << "switch ";
    p->expr->accept(this);
    cout << endl;
    for (auto c : p->cases) {
        c->accept(this);
    }
    if (p->defaultBody) {
        cout << "default" << endl;
        p->defaultBody->accept(this);
    }
    cout << "endswitch" << endl;
    return 0;
}



void PrintVisitor::imprimir(Program* programa){
    if (programa)
    {
        cout << "Codigo:" << endl; 
        programa->accept(this);
        cout << endl;
    }
    return ;
}

///////////////////////////////////////////////////////////////////////////////////
int EVALVisitor::visit(BinaryExp* exp) {
    int result;
    int v1 = exp->left->accept(this);
    int v2 = exp->right->accept(this);
    switch (exp->op) {
        case PLUS_OP:
            result = v1 + v2;
            break;
        case MINUS_OP:
            result = v1 - v2;
            break;
        case MUL_OP:
            result = v1 * v2;
            break;
        case DIV_OP:
            if (v2 != 0)
                result = v1 / v2;
            else {
                cout << "Error: división por cero" << endl;
                result = 0;
            }
            break;
        case POW_OP:
            result = pow(v1,v2);
            break;
        case LT_OP:
            result = (v1 < v2) ? 1 : 0;
            break;
        case GT_OP:
            result = (v1 > v2) ? 1 : 0;
            break;
        case LE_OP:
            result = (v1 <= v2) ? 1 : 0;
            break;
        case GE_OP:
            result = (v1 >= v2) ? 1 : 0;
            break;
        case EQ_OP:
            result = (v1 == v2) ? 1 : 0;
            break;
        case NE_OP:
            result = (v1 != v2) ? 1 : 0;
            break;
        case AND_OP:
            result = (v1 && v2) ? 1 : 0;
            break;
        case OR_OP:
            result = (v1 || v2) ? 1 : 0;
            break;
        default:
            cout << "Operador desconocido" << endl;
            result = 0;
    }
    return result;
}

int EVALVisitor::visit(NumberExp* exp) {
    return exp->value;
}

int EVALVisitor::visit(SqrtExp* exp) {
    return floor(sqrt( exp->value->accept(this)));
}

int EVALVisitor::visit(BoolExp* exp) {
    return exp->value ? 1 : 0;
}

void EVALVisitor::interprete(Program* programa){
    if (programa)
    {
        cout << "Interprete:" << endl;
        programa->accept(this);
    }
    return;

}

int EVALVisitor::visit(Program* p) {
    for (auto i:p->cuerpo)
    {
       i->accept(this);
    }
    
    return 0;
}

int EVALVisitor::visit(PrintStatement* p) {
    cout << p->valor->accept(this) << endl;
    return 0;
}

int EVALVisitor::visit(AssignStatement* p) {
    memoria[p->variable] = p->valor->accept(this);
    return 0;
}



int EVALVisitor::visit(IdExp* p) {
    return memoria[p->value];
}

int EVALVisitor::visit(Body* p) {
    for (auto stm : p->list_stm) {
        stm->accept(this);
    }
    return 0;
}

int EVALVisitor::visit(IfStatement* p) {
    if (p->condition->accept(this)) {
        p->ifbody->accept(this);
    } else {
        bool executed = false;
        for (auto elif : p->elif_list) {
            if (elif->condition->accept(this)) {
                elif->elifbody->accept(this);
                executed = true;
                break;
            }
        }
        if (!executed && p->elsebody) {
            p->elsebody->accept(this);
        }
    }
    return 0;
}

int EVALVisitor::visit(ElifStatement* p) {
    p->elifbody->accept(this);
    return 0;
}

int EVALVisitor::visit(DoWhileStatement* p) {
    do {
        try {
            p->body->accept(this);
        } catch (BreakSignal&) {
            // break dentro del body de do-while (e.g. dentro de un switch)
            // no debe romper el do-while, se propaga desde switch
        }
    } while (p->condition->accept(this));
    return 0;
}

int EVALVisitor::visit(WhileStatement* p) {
    while (p->condition->accept(this)) {
        try {
            p->body->accept(this);
        } catch (BreakSignal&) {
            // break dentro del body (e.g. dentro de un switch)
        }
    }
    return 0;
}

int EVALVisitor::visit(BreakStatement* p) {
    throw BreakSignal();
    return 0;
}

int EVALVisitor::visit(CaseStatement* p) {
    p->body->accept(this);
    if (p->hasBreak) {
        throw BreakSignal();
    }
    return 0;
}

int EVALVisitor::visit(SwitchStatement* p) {
    int val = p->expr->accept(this);
    bool matched = false;
    for (auto c : p->cases) {
        int caseVal = c->value->accept(this);
        if (val == caseVal) {
            matched = true;
            try {
                c->accept(this);
            } catch (BreakSignal&) {
                return 0;  // break sale del switch
            }
            // fall-through: si no hubo break, continuar con siguientes cases
        }
    }
    if (!matched && p->defaultBody) {
        try {
            p->defaultBody->accept(this);
        } catch (BreakSignal&) {
            return 0;
        }
    }
    return 0;
}
