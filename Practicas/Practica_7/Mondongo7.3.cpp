#include <iostream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string>
#include <cctype>
#include <regex>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#define archivo1 "Program.txt"
using namespace std;

// Estructura para tokens
struct Token {
    string lexema;
    string tipo;
};

// Estructura para el programa
struct MainProgram {
    string linea;
};

// Variables globales
vector<MainProgram> v_MP;
vector<Token> tokens;

// Array de palabras reservadas
string palabrasReservadas[] = {
    "if", "else", "for", "while", "int", "float", "bool", 
    "char", "string", "true", "false", "input", "write",
    "return", "include", "system", "aputs_agets"
};
int numPalabrasReservadas = 18;

set<char> valid_symbols = {
    '+', '-', '*', '/', '%', '<', '>', '=', '!', 
    '&', '|', '^', '~', ';', ',', '.', ':', '(', 
    ')', '[', ']', '{', '}', '?', '"', '#'
};

set<string> valid_symbols2 = {
    "++", "--", "==", "!=", "<=", ">=", 
    "&&", "||", "+=", ":="
};

void error() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << endl;
    cout << "Ingrese un dato válido" << endl;
    system("pause");
    system("cls");
}

// Función para verificar si es palabra reservada
bool esPalabraReservada(string palabra) {
    for(int i = 0; i < numPalabrasReservadas; i++) {
        if(palabrasReservadas[i] == palabra)
            return true;
    }
    return false;
}

// Función para verificar si es un número
bool esNumero(string str) {
    if(str.empty()) return false;
    for(int i = 0; i < str.length(); i++) {
        if(!isdigit(str[i]) && str[i] != '.') 
            return false;
    }
    return true;
}

// Función para verificar si es un identificador válido
bool esIdentificador(string str) {
    if(str.empty() || !isalpha(str[0])) 
        return false;
    for(int i = 0; i < str.length(); i++) {
        if(!isalnum(str[i]) && str[i] != '_') 
            return false;
    }
    return true;
}

// Función para obtener el tipo de token (OPTIMIZADA)
string obtenerTipo(string lexema) {
    // Verificar si es símbolo de dos caracteres
    if(lexema.length() == 2 && valid_symbols2.find(lexema) != valid_symbols2.end()) {
        return "Simb";
    }
    
    // Verificar si es símbolo de un carácter
    if(lexema.length() == 1 && valid_symbols.find(lexema[0]) != valid_symbols.end()) {
        return "Simb";
    }
    
    // Palabras reservadas
    if(esPalabraReservada(lexema)) {
        return "PR";
    }
    
    // Números
    if(esNumero(lexema)) {
        return "Num";
    }
    
    // Cadenas entre comillas
    if(lexema.length() >= 2 && lexema[0] == '"' && lexema[lexema.length()-1] == '"') {
        return "Cadena";
    }
    
    // Identificadores (variables)
    if(esIdentificador(lexema)) {
        return "Var";
    }
    
    return "Desconocido";
}

// Función para agregar un token
void agregarToken(string lexema, string tipo) {
    Token t;
    t.lexema = lexema;
    t.tipo = tipo;
    tokens.push_back(t);
}

// Función para analizar léxicamente (OPTIMIZADA)
void analizarLexico(string linea) {
    string palabra = "";
    bool dentroComillas = false;
    string cadena = "";
    
    for(int i = 0; i < linea.length(); i++) {
        char c = linea[i];
        
        // Manejo de cadenas entre comillas
        if(c == '"') {
            if(!dentroComillas) {
                if(!palabra.empty()) {
                    agregarToken(palabra, obtenerTipo(palabra));
                    palabra = "";
                }
                dentroComillas = true;
                cadena = "\"";
            } else {
                cadena += "\"";
                agregarToken(cadena, "Cadena");
                cadena = "";
                dentroComillas = false;
            }
            continue;
        }
        
        if(dentroComillas) {
            cadena += c;
            continue;
        }
        
        // Manejo de operadores compuestos (OPTIMIZADO con set)
        if(i < linea.length() - 1) {
            string dobleChar = "";
            dobleChar += c;
            dobleChar += linea[i+1];
            
            // Verificar si está en el set de símbolos de 2 caracteres
            if(valid_symbols2.find(dobleChar) != valid_symbols2.end()) {
                if(!palabra.empty()) {
                    agregarToken(palabra, obtenerTipo(palabra));
                    palabra = "";
                }
                if(dobleChar != "//") { // Ignorar comentarios
                    agregarToken(dobleChar, "Simb");
                }
                i++;
                continue;
            }
        }
        
        // Símbolos individuales (OPTIMIZADO con set)
        if(valid_symbols.find(c) != valid_symbols.end()) {
            if(!palabra.empty()) {
                agregarToken(palabra, obtenerTipo(palabra));
                palabra = "";
            }
            string simbolo = "";
            simbolo += c;
            agregarToken(simbolo, "Simb");
        }
        // Espacios en blanco
        else if(isspace(c)) {
            if(!palabra.empty()) {
                agregarToken(palabra, obtenerTipo(palabra));
                palabra = "";
            }
        }
        // Construir palabra
        else {
            palabra += c;
        }
    }
    
    // Agregar última palabra de la línea
    if(!palabra.empty()) {
        agregarToken(palabra, obtenerTipo(palabra));
        palabra = "";
    }
}

// Función para imprimir la tabla de tokens
void imprimirTabla() {
    cout << "\n╔═══════════════════╦═══════════╦═══════════════════╦═══════════╗" << endl;
    cout << "║ " << setw(17) << left << "Token" 
         << " ║ " << setw(9) << "Tipo"
         << " ║ " << setw(17) << "Token"
         << " ║ " << setw(9) << "Tipo" << " ║" << endl;
    cout << "╠═══════════════════╬═══════════╬═══════════════════╬═══════════╣" << endl;
    
    for(int i = 0; i < tokens.size(); i += 2) {
        cout << "║ " << setw(17) << left << tokens[i].lexema 
             << " ║ " << setw(9) << tokens[i].tipo << " ║ ";
        
        if(i + 1 < tokens.size()) {
            cout << setw(17) << left << tokens[i+1].lexema 
                 << " ║ " << setw(9) << tokens[i+1].tipo << " ║" << endl;
        } else {
            cout << setw(17) << " " << " ║ " << setw(9) << " " << " ║" << endl;
        }
    }
    cout << "╚═══════════════════╩═══════════╩═══════════════════╩═══════════╝" << endl;
    
    cout << "\nTotal de tokens encontrados: " << tokens.size() << endl;
}

// Función para guardar la tabla en archivo
void guardarTabla(string nombreArchivo) {
    ofstream salida(nombreArchivo.c_str());
    
    salida << "Token\tTipo\tToken\tTipo\n";
    
    for(int i = 0; i < tokens.size(); i += 2) {
        salida << tokens[i].lexema << "\t" << tokens[i].tipo << "\t";
        if(i + 1 < tokens.size()) {
            salida << tokens[i+1].lexema << "\t" << tokens[i+1].tipo << "\n";
        } else {
            salida << "\t\n";
        }
    }
    
    salida.close();
    cout << "\n✓ Tabla guardada en: " << nombreArchivo << endl;
}

void loadPGM() {
    cout << "Cargando el programa..." << endl;
    cout << "══════════════════════════════════════════════════" << endl;
    
    ifstream archPGM(archivo1);
    if (!archPGM.is_open()) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    
    // Limpiar vectores previos
    v_MP.clear();
    tokens.clear();
    
    string linea;
    
    // Saltar la primera línea si es necesario
    getline(archPGM, linea);
    
    // Leer todas las líneas
    while (getline(archPGM, linea)) {
        MainProgram data;
        data.linea = linea;
        v_MP.push_back(data);
        
        // Mostrar la línea
        cout << linea << endl;
        
        // Analizar léxicamente la línea
        analizarLexico(linea);
    }
    
    archPGM.close();
    
    cout << "══════════════════════════════════════════════════" << endl;
    cout << "¡Programa cargado exitosamente!" << endl;
    cout << endl;
}

void interfaz() {
    int seguir = 1;
    system("cls");
    
    do {
        do {
            system("cls");
            cout << "    ┌───────────────────────────────────────────┐" << endl;
            cout << "    |---| Práctica 7. Analizador sintáctico |---|" << endl;
            cout << "    └───────────────────────────────────────────┘" << endl;
            cout << endl;
            
            // Cargar el programa
            loadPGM();
            
            cout << endl;
            system("pause");
            system("cls");
            
            // Mostrar tabla de tokens
            cout << "    ┌───────────────────────────────────────────┐" << endl;
            cout << "    |--------| TABLA DE TOKENS |----------|" << endl;
            cout << "    └───────────────────────────────────────────┘" << endl;
            imprimirTabla();
            
            // Guardar en archivo
            guardarTabla("tokens_output.txt");
            
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

int main(int argc, char const *argv[]) {
    cout << "Bienvenido al Analizador Léxico y Sintáctico..." << endl;
    cout << endl;
    system("pause");
    interfaz();
    system("pause");
    return 0;
}