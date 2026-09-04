import os
import subprocess
import sys
 
# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp","parser.cpp","ast.cpp"]

# Compilar
compile = ["g++"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")

# Ejecutar
input_dir = "inputs"
for i in range(1, 11): 
    filename = f"input{i}.txt"
    filepath = os.path.join(input_dir, filename)
    if os.path.isfile(filepath):
        print(f"Ejecutando {filename}")
        run_cmd = ["./a.out", filepath]
        run_result = subprocess.run(run_cmd, capture_output=True, text=True)
        if run_result.returncode != 0:
            print(run_result.stdout, end="")
            print(run_result.stderr, end="", file=sys.stderr)
            sys.exit(f"Falló {filename}")

        tokens_file = os.path.join(input_dir, f"input{i}_tokens.txt")
        if not os.path.isfile(tokens_file):
            sys.exit(f"No se generó {tokens_file}")

        # Si el programa genera ast.dot, convertirlo a imagen en inputs/
        if os.path.isfile("ast.dot"):
            output_img = os.path.join(input_dir, f"ast_{i}.png")
            dot_cmd = ["dot", "-Tpng", "ast.dot", "-o", output_img]
            print(f"Generando {output_img}")
            dot_result = subprocess.run(dot_cmd, capture_output=True, text=True)
            if dot_result.returncode != 0:
                print(dot_result.stderr, end="", file=sys.stderr)
                sys.exit(f"No se pudo generar {output_img}")
            if not os.path.isfile(output_img):
                sys.exit(f"No se generó {output_img}")
    else:
        print(filename, "no encontrado en", input_dir)
