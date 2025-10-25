#include <iostream>
#include <stdlib.h>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#define archivo1 "Program.txt"
using namespace std;

// ============= ESTRUCTURAS =============

struct Symbol {
    string nombre;
    string tipo;
    int linea;
    string scope;
};

struct ErrorSemantico {
    int linea;
    string tipo;
    string mensaje;
};

struct MainProgram {
    string linea;
    int numeroLinea;
};

// ============= VARIABLES GLOBALES =============
vector<MainProgram> v_MP;
vector<Symbol> tablaSimbolos;
vector<ErrorSemantico> errores;
string scopeActual = "global";
int contadorScope = 0;

// ============= FUNCIONES AUXILIARES =============

void limpiarEspacios(string &str) {
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}

void agregarError(int linea, string tipo, string mensaje) {
    ErrorSemantico err;
    err.linea = linea;
    err.tipo = tipo;
    err.mensaje = mensaje;
    errores.push_back(err);
}

bool existeVariable(string nombreVar) {
    for (const auto& simbolo : tablaSimbolos) {
        if (simbolo.nombre == nombreVar) {
            return true;
        }
    }
    return false;
}

string obtenerTipoVariable(string nombreVar) {
    for (int i = tablaSimbolos.size() - 1; i >= 0; i--) {
        if (tablaSimbolos[i].nombre == nombreVar) {
            return tablaSimbolos[i].tipo;
        }
    }
    return "";
}

void agregarSimbolo(string nombre, string tipo, int linea, string scope) {
    Symbol simbolo;
    simbolo.nombre = nombre;
    simbolo.tipo = tipo;
    simbolo.linea = linea;
    simbolo.scope = scope;
    tablaSimbolos.push_back(simbolo);
}

// ============= VALIDACIÓN DE PRODUCCIONES DE LA GRAMÁTICA =============

// <numeric> ::= int <word> = <nums>; | int <word> = var; | float <word> = <float>;
bool esDeclaracionNumeric(string linea, int numLinea) {
    // int <word> = <nums>;
    regex patternInt(R"(int\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*(-?\d+)\s*;)");
    // int <word> = <id>; (variable)
    regex patternIntVar(R"(int\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*;)");
    // float <word> = <nums>.<nums>;
    regex patternFloat(R"(float\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*(-?\d+\.?\d*)\s*;)");
    // Declaración sin inicialización
    regex patternSinInit(R"((int|float)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;)");
    
    smatch match;
    
    if (regex_search(linea, match, patternInt)) {
        string nombreVar = match[1];
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Variable '" + nombreVar + "' ya fue declarada");
            return false;
        }
        agregarSimbolo(nombreVar, "int", numLinea, scopeActual);
        return true;
    }
    
    if (regex_search(linea, match, patternIntVar)) {
        string nombreVar = match[1];
        string varAsignada = match[2];
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Variable '" + nombreVar + "' ya fue declarada");
            return false;
        }
        if (!existeVariable(varAsignada)) {
            agregarError(numLinea, "no_declarada", 
                "Variable '" + varAsignada + "' no ha sido declarada");
        }
        agregarSimbolo(nombreVar, "int", numLinea, scopeActual);
        return true;
    }
    
    if (regex_search(linea, match, patternFloat)) {
        string nombreVar = match[1];
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Variable '" + nombreVar + "' ya fue declarada");
            return false;
        }
        agregarSimbolo(nombreVar, "float", numLinea, scopeActual);
        return true;
    }
    
    if (regex_search(linea, match, patternSinInit)) {
        string tipo = match[1];
        string nombreVar = match[2];
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Variable '" + nombreVar + "' ya fue declarada");
            return false;
        }
        agregarSimbolo(nombreVar, tipo, numLinea, scopeActual);
        return true;
    }
    
    return false;
}

// <boolean> ::= bool <word> = <bool_status>;
bool esDeclaracionBoolean(string linea, int numLinea) {
    regex pattern(R"(bool\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*(true|false)\s*;)");
    regex patternSinInit(R"(bool\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;)");
    smatch match;
    
    if (regex_search(linea, match, pattern)) {
        string nombreVar = match[1];
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Variable '" + nombreVar + "' ya fue declarada");
            return false;
        }
        agregarSimbolo(nombreVar, "bool", numLinea, scopeActual);
        return true;
    }
    
    if (regex_search(linea, match, patternSinInit)) {
        string nombreVar = match[1];
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Variable '" + nombreVar + "' ya fue declarada");
            return false;
        }
        agregarSimbolo(nombreVar, "bool", numLinea, scopeActual);
        return true;
    }
    
    return false;
}

// <alpha_num> ::= string = <word>;
bool esDeclaracionString(string linea, int numLinea) {
    regex pattern(R"(string\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;)");
    smatch match;
    
    if (regex_search(linea, match, pattern)) {
        string nombreVar = match[1];
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Variable '" + nombreVar + "' ya fue declarada");
            return false;
        }
        agregarSimbolo(nombreVar, "string", numLinea, scopeActual);
        return true;
    }
    return false;
}

// <input_var> ::= input(<ListIds>);
bool esInput(string linea, int numLinea) {
    regex pattern(R"(input\s*\(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\)\s*;)");
    smatch match;
    
    if (regex_search(linea, match, pattern)) {
        string nombreVar = match[1];
        if (!existeVariable(nombreVar)) {
            agregarError(numLinea, "no_declarada", 
                "Variable '" + nombreVar + "' usada en input() no ha sido declarada");
        }
        return true;
    }
    return false;
}

// <output_var> ::= write(<List_output>);
bool esWrite(string linea, int numLinea) {
    regex pattern(R"(write\s*\([^)]*\)\s*;)");
    
    if (regex_search(linea, pattern)) {
        // Extraer variables dentro del write
        regex varPattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
        smatch matches;
        string temp = linea;
        
        while (regex_search(temp, matches, varPattern)) {
            string posibleVar = matches[0];
            // Ignorar palabras reservadas
            if (posibleVar != "write" && posibleVar != "true" && posibleVar != "false") {
                // Verificar si es una variable (no es un número)
                if (!regex_match(posibleVar, regex(R"(\d+)"))) {
                    if (existeVariable(posibleVar)) {
                        // Variable válida
                    }
                }
            }
            temp = matches.suffix();
        }
        return true;
    }
    return false;
}

// <sent_asign> - Asignación sin declaración
bool esAsignacion(string linea, int numLinea) {
    // Detectar tipo += -= etc (error común)
    regex errorPattern(R"((int|float|bool|string)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*[\+\-\*\/]=)");
    smatch matchError;
    
    if (regex_search(linea, matchError, errorPattern)) {
        string tipo = matchError[1];
        string nombreVar = matchError[2];
        agregarError(numLinea, "error_sintaxis", 
            "No puede usar '" + tipo + "' con operador compuesto. Variable '" + 
            nombreVar + "' ya debería estar declarada. Elimine el tipo.");
        return false;
    }
    
    // Asignación normal
    regex pattern(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*[\+\-\*\/]?=\s*[^;]+;)");
    smatch match;
    
    if (regex_search(linea, match, pattern)) {
        string nombreVar = match[1];
        if (!existeVariable(nombreVar)) {
            agregarError(numLinea, "no_declarada", 
                "Variable '" + nombreVar + "' no ha sido declarada antes de asignarle valor");
        }
        return true;
    }
    return false;
}

// <sent_if> ::= if (<exp_log>) {<set_sentc>}
bool esIf(string linea, int numLinea) {
    regex pattern(R"(if\s*\([^)]+\)\s*\{?)");
    
    if (regex_search(linea, pattern)) {
        contadorScope++;
        scopeActual = "if_" + to_string(contadorScope);
        return true;
    }
    return false;
}

// <else> ::= else {<set_sentc>}
bool esElse(string linea, int numLinea) {
    regex pattern(R"(^\s*}\s*else\s+if|^\s*else\s+if)");
    regex patternElse(R"(^\s*}\s*else\s*\{|^\s*else\s*\{)");
    
    if (regex_search(linea, pattern)) {
        contadorScope++;
        scopeActual = "elseif_" + to_string(contadorScope);
        return true;
    }
    
    if (regex_search(linea, patternElse)) {
        contadorScope++;
        scopeActual = "else_" + to_string(contadorScope);
        return true;
    }
    return false;
}

// <sent_for> ::= for (<numeric> ; <exp_log> ; <inc>){<set_sentc>}
bool esFor(string linea, int numLinea) {
    // for(int i = 0; ...)
    regex patternConDeclaracion(R"(for\s*\(\s*int\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    // for(i = 0; ...) sin declaración
    regex patternSinDeclaracion(R"(for\s*\(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*=)");
    
    smatch match;
    
    if (regex_search(linea, match, patternConDeclaracion)) {
        string nombreVar = match[1];
        contadorScope++;
        scopeActual = "for_" + to_string(contadorScope);
        agregarSimbolo(nombreVar, "int", numLinea, scopeActual);
        return true;
    }
    
    if (regex_search(linea, match, patternSinDeclaracion)) {
        string nombreVar = match[1];
        if (!existeVariable(nombreVar)) {
            agregarError(numLinea, "no_declarada", 
                "Variable '" + nombreVar + "' en for no ha sido declarada");
        }
        contadorScope++;
        scopeActual = "for_" + to_string(contadorScope);
        return true;
    }
    
    return false;
}

bool esLlaveCierre(string linea) {
    limpiarEspacios(linea);
    if (linea == "}") {
        scopeActual = "global";
        return true;
    }
    return false;
}

bool esReturn(string linea) {
    return linea.find("return") != string::npos;
}

bool esSystemPause(string linea) {
    return linea.find("system") != string::npos;
}

// ============= ANÁLISIS PRINCIPAL =============

void analizarLinea(string linea, int numLinea) {
    limpiarEspacios(linea);
    
    if (linea.empty() || linea.find("#include") != string::npos) {
        return;
    }
    
    // Verificar producciones de la gramática
    if (esDeclaracionNumeric(linea, numLinea)) return;
    if (esDeclaracionBoolean(linea, numLinea)) return;
    if (esDeclaracionString(linea, numLinea)) return;
    if (esInput(linea, numLinea)) return;
    if (esWrite(linea, numLinea)) return;
    if (esIf(linea, numLinea)) return;
    if (esElse(linea, numLinea)) return;
    if (esFor(linea, numLinea)) return;
    if (esLlaveCierre(linea)) return;
    if (esReturn(linea)) return;
    if (esSystemPause(linea)) return;
    if (esAsignacion(linea, numLinea)) return;
    
    // Si no coincide con ninguna producción
    if (linea != "{" && linea != "}") {
        agregarError(numLinea, "sintaxis", 
            "La línea no coincide con ninguna producción de la gramática");
    }
}

void analizarSemantica() {
    cout << "\n========== INICIANDO ANÁLISIS SEMÁNTICO ==========" << endl;
    cout << "Validando contra gramática BNF...\n" << endl;
    
    tablaSimbolos.clear();
    errores.clear();
    scopeActual = "global";
    contadorScope = 0;
    
    for (const auto& programa : v_MP) {
        analizarLinea(programa.linea, programa.numeroLinea);
    }
}

void mostrarTablaSimbolos() {
    cout << "\n========== TABLA DE SÍMBOLOS ==========" << endl;
    cout << "| Nombre       | Tipo    | Línea | Scope       |" << endl;
    cout << "|--------------|---------|-------|-------------|" << endl;
    
    for (const auto& simbolo : tablaSimbolos) {
        cout << "| " << simbolo.nombre;
        for (int i = simbolo.nombre.length(); i < 13; i++) cout << " ";
        cout << "| " << simbolo.tipo;
        for (int i = simbolo.tipo.length(); i < 8; i++) cout << " ";
        cout << "| " << simbolo.linea;
        string lineaStr = to_string(simbolo.linea);
        for (int i = lineaStr.length(); i < 6; i++) cout << " ";
        cout << "| " << simbolo.scope;
        for (int i = simbolo.scope.length(); i < 12; i++) cout << " ";
        cout << "|" << endl;
    }
    cout << "========================================" << endl;
}

void mostrarErrores() {
    cout << "\n========== ERRORES SEMÁNTICOS ==========" << endl;
    
    if (errores.empty()) {
        cout << "✓ El programa cumple con la gramática y no tiene errores semánticos." << endl;
    } else {
        cout << "✗ Se encontraron " << errores.size() << " error(es):\n" << endl;
        for (const auto& error : errores) {
            cout << "  [Línea " << error.linea << "] (" << error.tipo << ")" << endl;
            cout << "  → " << error.mensaje << "\n" << endl;
        }
    }
    cout << "========================================" << endl;
}

void loadPGM() {
    cout << "Cargando programa desde " << archivo1 << "...\n" << endl;
    ifstream archPGM(archivo1);
    if (!archPGM.is_open()) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    
    v_MP.clear();
    string linea;
    int numLinea = 0;
    
    while (getline(archPGM, linea)) {
        numLinea++;
        MainProgram data;
        data.linea = linea;
        data.numeroLinea = numLinea;
        v_MP.push_back(data);
        
        cout << "[" << numLinea << "] " << linea << endl;
    }
    
    archPGM.close();
    cout << "\n¡Programa cargado!" << endl;
}

void error() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << "Ingrese un dato válido" << endl;
    system("pause");
    system("cls");
}

void interfaz() {
    int seguir = 1;
    system("cls");
    
    do {
        do {
            system("cls");
            cout << "    ┌────────────────────────────────────────────────┐" << endl;
            cout << "    │  Práctica 7 - Analizador Semántico con BNF    │" << endl;
            cout << "    └────────────────────────────────────────────────┘" << endl;
            cout << endl;
            
            loadPGM();
            analizarSemantica();
            mostrarTablaSimbolos();
            mostrarErrores();
            
            cout << endl;
            cout << "  ¿Desea volver a analizar? 1)Sí 2)No: ";
            cin >> seguir;
            cout << endl;
            if (cin.fail()) error();
        } while (cin.fail() || (seguir != 1 && seguir != 2));
        system("cls");
    } while (seguir == 1);
}

int main(int argc, char const *argv[]) {
    cout << "Bienvenido al Analizador Semántico...\n" << endl;
    interfaz();
    system("pause");
    return 0;
}