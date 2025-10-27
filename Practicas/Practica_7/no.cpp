// ============================================
// SISTEMA DE MANEJO DE ERRORES MEJORADO
// ============================================

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Estructura para almacenar errores léxicos
struct ErrorLexico {
    int codigo;
    string descripcion;
    int linea;
    string lexema;
};

// Array de definiciones de errores
struct TipoError {
    int codigo;
    string nombre;
    string descripcion;
};

// Catálogo de errores léxicos
const TipoError CATALOGO_ERRORES[] = {
    {1, "CADENA_NO_CERRADA", "Cadena literal sin comillas de cierre"},
    {2, "CARACTER_INVALIDO", "Carácter no reconocido en el lenguaje"},
    {3, "NUMERO_INVALIDO", "Formato de número incorrecto"},
    {4, "IDENTIFICADOR_INVALIDO", "Identificador con formato inválido"},
    {5, "OPERADOR_INCOMPLETO", "Operador de dos caracteres incompleto"},
    {6, "CADENA_VACIA", "Cadena vacía sin contenido"},
    {7, "NUMERO_MULTIPUNTO", "Número con más de un punto decimal"}
};

// Variables globales para control de errores
vector<ErrorLexico> erroresLexicos;
bool errorEncontrado = false;

// Función para registrar un error léxico
void registrarError(int codigoError, int linea, string lexema) {
    ErrorLexico error;
    error.codigo = codigoError;
    error.linea = linea;
    error.lexema = lexema;
    
    // Buscar descripción del error
    int numErrores = sizeof(CATALOGO_ERRORES) / sizeof(CATALOGO_ERRORES[0]);
    for(int i = 0; i < numErrores; i++) {
        if(CATALOGO_ERRORES[i].codigo == codigoError) {
            error.descripcion = CATALOGO_ERRORES[i].descripcion;
            break;
        }
    }
    
    erroresLexicos.push_back(error);
    errorEncontrado = true;
}

// Función mejorada para validar números
bool esNumeroValido(string str, int linea) {
    if(str.empty()) return false;
    
    bool tienePunto = false;
    int inicio = 0;
    
    // Permitir signo al inicio
    if(str[0] == '+' || str[0] == '-') {
        inicio = 1;
        if(str.length() == 1) {
            registrarError(3, linea, str);
            return false;
        }
    }
    
    for(int i = inicio; i < str.length(); i++) {
        if(str[i] == '.') {
            if(tienePunto) {
                registrarError(7, linea, str); // Múltiples puntos
                return false;
            }
            tienePunto = true;
        } else if(!isdigit(str[i])) {
            registrarError(3, linea, str);
            return false;
        }
    }
    
    // No puede terminar o empezar solo con punto
    if(str == "." || str == "+." || str == "-.") {
        registrarError(3, linea, str);
        return false;
    }
    
    return true;
}

// Función mejorada para validar identificadores
bool esIdentificadorValido(string str, int linea) {
    if(str.empty() || !isalpha(str[0])) {
        registrarError(4, linea, str);
        return false;
    }
    
    for(int i = 0; i < str.length(); i++) {
        if(!isalnum(str[i]) && str[i] != '_') {
            registrarError(4, linea, str);
            return false;
        }
    }
    
    return true;
}

// Función mejorada de análisis léxico con detección de errores
void analisisLexicoMejorado(string line, int numLinea) {
    bool comillas = false;
    string palabra = "", cadena = "";
    
    for (int i = 0; i < line.length(); i++) {
        char car = line[i];

        // Manejo de cadenas literales
        if (car == '"') {
            if (!comillas) {
                if (!palabra.empty()) {
                    // Validar palabra antes de agregar
                    if(!esPalabraReservada(palabra) && 
                       !esNumeroValido(palabra, numLinea) && 
                       !esIdentificadorValido(palabra, numLinea)) {
                        registrarError(2, numLinea, palabra);
                        return; // Detener análisis
                    }
                    addToken(palabra, setType(palabra), numLinea);
                    palabra = "";
                }
                comillas = true;
                cadena = "\"";
            } else {
                cadena += "\"";
                // Validar que no sea cadena vacía
                if(cadena == "\"\"") {
                    registrarError(6, numLinea, cadena);
                    return;
                }
                addToken(cadena, "Cadena", numLinea);
                cadena = "";
                comillas = false;
            }
            continue;
        }
        
        if(comillas) {
            cadena += car;
            continue;
        }

        // Validar punto decimal en números
        if (car == '.' && !palabra.empty() && i + 1 < line.length() && isdigit(line[i+1])) {
            palabra += car;
            continue;
        }

        // Manejo de operadores dobles
        if (i < line.length() - 1) {
            string dobleChar = "";
            dobleChar += car;
            dobleChar += line[i+1];

            if(valid_symbols2.find(dobleChar) != valid_symbols2.end()) {
                if(!palabra.empty()) {
                    // Validar palabra
                    if(!esPalabraReservadas(palabra) && 
                       !esNumeroValido(palabra, numLinea) && 
                       !esIdentificadorValido(palabra, numLinea)) {
                        registrarError(2, numLinea, palabra);
                        return;
                    }
                    addToken(palabra, setType(palabra), numLinea);
                    palabra = "";
                }
                if(dobleChar != "//") {
                    addToken(dobleChar, setType(dobleChar), numLinea);
                }
                i++;
                continue;
            }
        }
        
        // Símbolos simples
        if(valid_symbols.find(car) != valid_symbols.end()) {
            if(!palabra.empty()) {
                // Validar palabra
                if(!esPalabraReservada(palabra) && 
                   !esNumeroValido(palabra, numLinea) && 
                   !esIdentificadorValido(palabra, numLinea)) {
                    registrarError(2, numLinea, palabra);
                    return;
                }
                addToken(palabra, setType(palabra), numLinea);
                palabra = "";
            }
            string simbolo = "";
            simbolo += car;
            addToken(simbolo, setType(simbolo), numLinea);
        } else if (isspace(car)) {
            if(!palabra.empty()) {
                // Validar palabra
                if(!esPalabraReservada(palabra) && 
                   !esNumeroValido(palabra, numLinea) && 
                   !esIdentificadorValido(palabra, numLinea)) {
                    registrarError(2, numLinea, palabra);
                    return;
                }
                addToken(palabra, setType(palabra), numLinea);
                palabra = "";
            }
        } else {
            // Validar si es un carácter válido
            if(!isalnum(car) && car != '_' && car != '.') {
                registrarError(2, numLinea, string(1, car));
                return;
            }
            palabra += car;
        }
    }
    
    // Verificar si quedó una cadena sin cerrar
    if(comillas) {
        registrarError(1, numLinea, cadena);
        return;
    }
    
    // Procesar última palabra
    if (!palabra.empty()) {
        if(!esPalabraReservada(palabra) && 
           !esNumeroValido(palabra, numLinea) && 
           !esIdentificadorValido(palabra, numLinea)) {
            registrarError(2, numLinea, palabra);
            return;
        }
        addToken(palabra, setType(palabra), numLinea);
    }
}

// Función para mostrar errores léxicos
void mostrarErroresLexicos() {
    if(erroresLexicos.empty()) return;
    
    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║          ERRORES LÉXICOS DETECTADOS                   ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝" << endl;
    
    for(int i = 0; i < erroresLexicos.size(); i++) {
        cout << "\n❌ ERROR #" << (i+1) << endl;
        cout << "   Código:      " << erroresLexicos[i].codigo << endl;
        cout << "   Línea:       " << erroresLexicos[i].linea << endl;
        cout << "   Lexema:      '" << erroresLexicos[i].lexema << "'" << endl;
        cout << "   Descripción: " << erroresLexicos[i].descripcion << endl;
        cout << "   ─────────────────────────────────────────────────────" << endl;
    }
    
    cout << "\n⚠️  Total de errores: " << erroresLexicos.size() << endl;
    cout << "    El análisis se ha detenido.\n" << endl;
}

// Función loadPGM modificada
void loadPGMMejorado() {
    cout << "Cargando el programa..." << endl;
    ifstream archPGM(archivo1);
    if (!archPGM.is_open()) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }

    string linea;
    int NoLine = 0;
    getline(archPGM, linea); // Saltar primera línea

    while (getline(archPGM, linea) && !errorEncontrado) {
        NoLine++;
        
        // Saltar líneas vacías
        bool esVacia = true;
        for(char c : linea) {
            if(!isspace(c)) {
                esVacia = false;
                break;
            }
        }
        if(esVacia) continue;
        
        MainProgram data;
        data.linea = linea;
        v_MP.push_back(data);

        cout << NoLine << ": " << linea << endl;
        analisisLexicoMejorado(linea, NoLine);
        
        // Si se encontró un error, detener
        if(errorEncontrado) {
            cout << "\n⛔ Análisis detenido por error léxico" << endl;
            archPGM.close();
            return;
        }
    }
    
    archPGM.close();
    cout << "\n✓ Programa cargado exitosamente!" << endl;
}

// Función MainProcess modificada
void MainProcessMejorado() {
    // Limpiar estructuras
    v_MP.clear();
    v_tokens.clear();
    Error.clear();
    erroresLexicos.clear();
    errorEncontrado = false;
    acumToken = 0;
    
    // Cargar programa
    loadPGMMejorado();
    
    // Si hubo errores léxicos, mostrarlos y salir
    if(errorEncontrado) {
        mostrarErroresLexicos();
        return;
    }
    
    // Mostrar tabla de tokens solo si no hay errores léxicos
    tokenTable();
    cout << "Total de tokens: " << v_tokens.size() << endl;

    // Realizar análisis sintáctico
    bool resultFinal = program();

    if (Error.empty()) {
        cout << "\n✓ Análisis sintáctico completado sin errores" << endl;
        cout << "✓ El programa es sintácticamente correcto" << endl;
    } else {
        cout << "\n⚠️  Se encontraron " << Error.size() << " errores sintácticos:" << endl;
        for (int i = 0; i < Error.size(); i++) {
            cout << (i+1) << ". " << Error[i] << endl;
        }
    }
}