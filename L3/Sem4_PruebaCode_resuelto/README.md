

## Gramática inicial

```
Program  ::= StmtList

StmtList ::= Stmt { ';' Stmt }*

Stmt     ::= Id '=' CExp  |  'print' '(' CExp ')'

CExp     ::= Expr { ('+' | '-') Expr }*

Expr     ::= Term { ('*' | '/') Term }*

Term     ::= Factor [ '**' Factor ]

Factor   ::= Number | '(' CExp ')' | 'sqrt' '(' CExp ')' | Id
```
## Gramática final 
```
Program  ::= StmtList

StmtList ::= Stmt { ';' Stmt }*

Stmt     ::= IdList '=' CExpList | 'print' '(' CExpList ')'

IdList   ::= Id { ',' Id }*

CExpList ::= CExp { ',' CExp }*

CExp     ::= Expr { ('+' | '-') Expr }*

Expr     ::= Term { ('*' | '/') Term }*

Term     ::= Factor [ '**' Factor ]

Factor   ::= Number
           | '(' CExp ')'
           | Id
           | 'sqrt' '(' CExp ')'
           | 'min' '(' CExp ',' CExp ')'
           | 'max' '(' CExp ',' CExp ')'
```
## Estructura del proyecto

```
scanner.h/.cpp   Analizador léxico 
token.h/.cpp     Definición de Token
parser.h/.cpp    Analizador sintáctico 
ast.h/.cpp       Nodos del AST
visitor.h/.cpp   Visitors de impresión y evaluación
main.cpp         Punto de entrada
inputs/          Programas de entrada de ejemplo
outputs/         Resultados generados 
run_all_inputs.py  Script para compilar y ejecutar todos los inputs
```

La implementación final admite listas de identificadores y expresiones,
asignaciones múltiples, impresión de varios valores y las funciones binarias
`min` y `max`. En una asignación, ambas listas deben tener la misma longitud y
todas las expresiones del lado derecho se evalúan antes de actualizar las
variables.

## Compilación y ejecución

```bash
g++ main.cpp scanner.cpp token.cpp parser.cpp ast.cpp visitor.cpp -o a.out
./a.out inputs/input1.txt
```

También se puede usar el script que compila y corre todos los archivos de
`inputs/`, guardando tokens, AST y salida en `outputs/`:

```bash
python run_all_inputs.py
```
