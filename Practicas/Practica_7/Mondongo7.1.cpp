#include <iostream>
#include <stdlib.h>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <regex>
#define archivo1 "Program.txt"
using namespace std;

// ============= ESTRUCTURAS =============

// Estructura para la tabla de símbolos
struct Symbol {
    string nombre;
    string tipo;        // "int", "float", "bool", "string", "char"
    int linea;          // línea donde fue declarada
    string scope;       // "global", "if", "else", "for"
    bool inicializada;  // si tiene valor asignado
};

// Estructura para errores semánticos
struct ErrorSemantico {
    int linea;
    string tipo;        // "redeclaracion", "no_declarada", "tipo_incompatible"
    string mensaje;
};

// Estructura para las líneas del programa
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

void error() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << endl;
    cout << "Ingrese un dato válido" << endl;
    system("pause");
    system("cls");
}

// Función para obtener el tipo de una variable desde la tabla de símbolos
string obtenerTipoVariable(string nombreVar) {
    // Buscar de atrás hacia adelante para respetar scope más cercano
    for (int i = tablaSimbolos.size() - 1; i >= 0; i--) {
        if (tablaSimbolos[i].nombre == nombreVar) {
            return tablaSimbolos[i].tipo;
        }
    }
    return ""; // No encontrada
}

// Función para verificar si una variable ya existe en el scope actual
bool existeEnScopeActual(string nombreVar, string scope) {
    for (const auto& simbolo : tablaSimbolos) {
        if (simbolo.nombre == nombreVar && simbolo.scope == scope) {
            return true;
        }
    }
    return false;
}

// Función para verificar si una variable existe en cualquier scope
bool existeVariable(string nombreVar) {
    for (const auto& simbolo : tablaSimbolos) {
        if (simbolo.nombre == nombreVar) {
            return true;
        }
    }
    return false;
}

// Agregar error semántico
void agregarError(int linea, string tipo, string mensaje) {
    ErrorSemantico err;
    err.linea = linea;
    err.tipo = tipo;
    err.mensaje = mensaje;
    errores.push_back(err);
}

// ============= FUNCIONES DE ANÁLISIS =============

// Detectar tipo de dato de una declaración
string detectarTipo(string linea) {
    if (linea.find("int ") != string::npos) return "int";
    if (linea.find("float ") != string::npos) return "float";
    if (linea.find("bool ") != string::npos) return "bool";
    if (linea.find("string ") != string::npos) return "string";
    if (linea.find("char ") != string::npos) return "char";
    return "";
}

// Extraer nombre de variable de una declaración
string extraerNombreVariable(string linea, string tipo) {
    size_t pos = linea.find(tipo);
    if (pos == string::npos) return "";
    
    string resto = linea.substr(pos + tipo.length());
    // Eliminar espacios iniciales
    resto.erase(0, resto.find_first_not_of(" \t"));
    
    // Extraer hasta el primer espacio, =, ;, (, o +
    size_t fin = resto.find_first_of(" =;(+");
    if (fin != string::npos) {
        return resto.substr(0, fin);
    }
    return resto;
}

// Detectar scope basado en palabras clave
void actualizarScope(string linea) {
    if (linea.find("if") != string::npos || linea.find("else if") != string::npos) {
        contadorScope++;
        scopeActual = "if_" + to_string(contadorScope);
    } else if (linea.find("else") != string::npos && linea.find("else if") == string::npos) {
        contadorScope++;
        scopeActual = "else_" + to_string(contadorScope);
    } else if (linea.find("for") != string::npos) {
        contadorScope++;
        scopeActual = "for_" + to_string(contadorScope);
    } else if (linea.find("}") != string::npos) {
        // Salir del scope (simplificado)
        scopeActual = "global";
    }
}

// Analizar declaración de variables
void analizarDeclaracion(string linea, int numLinea) {
    string tipo = detectarTipo(linea);
    if (tipo.empty()) return;
    
    string nombreVar = extraerNombreVariable(linea, tipo);
    if (nombreVar.empty()) return;
    
    // Verificar si es una redeclaración con tipo (ej: "float porcentaje += 0.05")
    if (linea.find("+=") != string::npos || linea.find("-=") != string::npos || 
        linea.find("*=") != string::npos || linea.find("/=") != string::npos) {
        
        if (existeVariable(nombreVar)) {
            agregarError(numLinea, "redeclaracion", 
                "Error: Redeclaración de variable '" + nombreVar + 
                "'. Ya fue declarada previamente. No use el tipo en asignaciones compuestas.");
            return;
        }
    }
    
    // Verificar redeclaración en el mismo scope
    if (existeEnScopeActual(nombreVar, scopeActual)) {
        agregarError(numLinea, "redeclaracion", 
            "Error: Variable '" + nombreVar + "' ya fue declarada en el scope actual (línea " + 
            to_string(numLinea) + ")");
        return;
    }
    
    // Agregar a la tabla de símbolos
    Symbol nuevoSimbolo;
    nuevoSimbolo.nombre = nombreVar;
    nuevoSimbolo.tipo = tipo;
    nuevoSimbolo.linea = numLinea;
    nuevoSimbolo.scope = scopeActual;
    nuevoSimbolo.inicializada = (linea.find("=") != string::npos);
    
    tablaSimbolos.push_back(nuevoSimbolo);
}

// Analizar uso de variables (en expresiones, for, etc.)
void analizarUsoVariables(string linea, int numLinea) {
    // Detectar variables en bucles for sin declaración (ej: for(i = 0...))
    if (linea.find("for") != string::npos) {
        regex forRegex(R"(for\s*\(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*=)");
        smatch match;
        if (regex_search(linea, match, forRegex)) {
            string varFor = match[1];
            // Verificar si NO es una declaración (no tiene tipo antes)
            string tipos[] = {"int", "float", "bool", "string", "char"};
            bool esDeclaracion = false;
            for (const string& tipo : tipos) {
                if (linea.find(tipo + " " + varFor) != string::npos) {
                    esDeclaracion = true;
                    break;
                }
            }
            
            if (!esDeclaracion && !existeVariable(varFor)) {
                agregarError(numLinea, "no_declarada", 
                    "Error: Variable '" + varFor + 
                    "' usada en for sin declaración previa (línea " + to_string(numLinea) + ")");
            }
        }
    }
    
    // Detectar asignaciones a variables no declaradas
    regex asignacionRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*[\+\-\*\/]?=)");
    smatch matches;
    string lineaTemp = linea;
    
    while (regex_search(lineaTemp, matches, asignacionRegex)) {
        string varAsignada = matches[1];
        
        // Ignorar palabras clave
        vector<string> palabrasClave = {"if", "else", "for", "while", "return", "int", "float", "bool", "string", "char"};
        bool esPalabraClave = false;
        for (const string& palabra : palabrasClave) {
            if (varAsignada == palabra) {
                esPalabraClave = true;
                break;
            }
        }
        
        if (!esPalabraClave && !existeVariable(varAsignada)) {
            // Verificar que no sea una declaración en la misma línea
            bool esDeclaracion = false;
            string tipos[] = {"int", "float", "bool", "string", "char"};
            for (const string& tipo : tipos) {
                if (linea.find(tipo + " " + varAsignada) != string::npos) {
                    esDeclaracion = true;
                    break;
                }
            }
            
            if (!esDeclaracion) {
                agregarError(numLinea, "no_declarada", 
                    "Error: Variable '" + varAsignada + 
                    "' no ha sido declarada (línea " + to_string(numLinea) + ")");
            }
        }
        
        lineaTemp = matches.suffix();
    }
}

// Función principal de análisis semántico
void analizarSemantica() {
    cout << "\n========== INICIANDO ANÁLISIS SEMÁNTICO ==========" << endl;
    
    // Limpiar estructuras previas
    tablaSimbolos.clear();
    errores.clear();
    scopeActual = "global";
    contadorScope = 0;
    
    // Primera pasada: construir tabla de símbolos
    for (const auto& programa : v_MP) {
        actualizarScope(programa.linea);
        analizarDeclaracion(programa.linea, programa.numeroLinea);
    }
    
    // Reiniciar scope para segunda pasada
    scopeActual = "global";
    contadorScope = 0;
    
    // Segunda pasada: verificar uso de variables
    for (const auto& programa : v_MP) {
        actualizarScope(programa.linea);
        analizarUsoVariables(programa.linea, programa.numeroLinea);
    }
}

// Mostrar tabla de símbolos
void mostrarTablaSimbolos() {
    cout << "\n========== TABLA DE SÍMBOLOS ==========" << endl;
    cout << "| Nombre       | Tipo    | Línea | Scope       |" << endl;
    cout << "|--------------|---------|-------|-------------|" << endl;
    
    for (const auto& simbolo : tablaSimbolos) {
        cout << "| " << simbolo.nombre;
        // Padding
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

// Mostrar errores semánticos
void mostrarErrores() {
    cout << "\n========== ERRORES SEMÁNTICOS ==========" << endl;
    
    if (errores.empty()) {
        cout << "✓ No se encontraron errores semánticos." << endl;
    } else {
        cout << "✗ Se encontraron " << errores.size() << " error(es):" << endl << endl;
        for (const auto& error : errores) {
            cout << "  [Línea " << error.linea << "] " << error.mensaje << endl;
        }
    }
    cout << "========================================" << endl;
}

// ============= CARGA DE PROGRAMA =============

void loadPGM() {
    cout << "Cargando el programa..." << endl;
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
        if (linea.empty() || linea.find("#include") != string::npos) {
            continue; // Saltar líneas vacías y includes
        }
        
        MainProgram data;
        data.linea = linea;
        data.numeroLinea = numLinea;
        v_MP.push_back(data);
        
        cout << "[" << numLinea << "] " << linea << endl;
    }
    
    archPGM.close();
    cout << "\n¡Programa cargado exitosamente!" << endl;
}

// ============= INTERFAZ =============

void interfaz() {
    int seguir = 1;
    system("cls");
    
    do {
        do {
            system("cls");
            cout << "    ┌──────────────────────────────────────────┐" << endl;
            cout << "    |---| Práctica 7. Analizador semántico |---|" << endl;
            cout << "    └──────────────────────────────────────────┘" << endl;
            cout << endl;
            
            // Cargar programa
            loadPGM();
            
            // Realizar análisis semántico
            analizarSemantica();
            
            // Mostrar resultados
            mostrarTablaSimbolos();
            mostrarErrores();
            
            cout << endl;
            if (cin.fail()) error();
            cout << "  ¿Desea volver a evaluar el programa? 1)Sí 2)No: ";
            cin >> seguir;
            cout << endl;
            if (cin.fail()) error();
        } while (cin.fail() || (seguir != 1 && seguir != 2));
        system("cls");
    } while (seguir == 1);
}

// ============= MAIN =============

int main(int argc, char const *argv[]) {
    cout << "Bienvenido al Analizador Semántico..." << endl;
    cout << endl;
    interfaz();
    system("pause");
    return 0;
}