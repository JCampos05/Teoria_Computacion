#include <iostream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string> //manipulación de stings
#include <cctype>
#include <regex>
#include <vector>
#include <fstream> //librerias .csv
#include <sstream> //librerias .csv
#include <iomanip>
#include <set>  
#define archivo1 "test.txt"
using namespace std;

struct MainProgram {
    string linea;
};
vector <MainProgram> v_MP;

struct token {
    string lexema, tipo;
    int linea;
};
vector <token> v_tokens;
int acumToken = 0;
vector <string> Error;


struct setError { // Estructura para almacenar Errores léxicos
    int codigo;
    string tipo;
    string descripcion;
    int linea;
    string lexema;
    string reference;
};
vector<setError> ErroresLexicos;

struct TipoError { // Array de definiciones de Errores
    int codigo;
    string nombre;
    string descripcion;
};

const TipoError Errores[] = { // Catálogo de Errores léxicos
    {1, "CADENA_NO_CERRADA", "Cadena literal sin comillas de cierre"},
    {2, "CARACTER_INVALIDO", "Carácter no válido en el lenguaje"},
    {3, "NUMERO_INVALIDO", "Formato numérico incorrecto"},
    {4, "IDENTIFICADOR_INVALIDO", "Identificador mal formado"},
    
    // Errores Sintácticos (11-30)
    {11, "SENTENCIA_INCOMPLETA", "Sentencia incompleta o mal formada"},
    {12, "FALTA_PUNTO_COMA", "Se esperaba ';' al final de la sentencia"},
    {13, "DECLARACION_INVALIDA", "Declaración de variable incompleta"},
    {14, "FALTA_ASIGNACION", "Falta el operador '=' en la declaración"},
    {15, "VALOR_FALTANTE", "Falta el valor después de '='"},
    {16, "TIPO_INVALIDO", "Tipo de dato no válido"},
    {17, "ESTRUCTURA_INVALIDA", "Estructura de control mal formada"},
    {18, "VARIABLE_SOLA", "Variable declarada sin inicialización"},
    {19, "EXPRESION_INVALIDA", "Expresión inválida o incompleta"}
};
bool matchError = false;

string palabrasReservadas[] = {
    "if", "else", "for", "while", "int", "float", "bool", 
    "char", "string", "true", "false", "input", "write",
    "return", "include", "system", "aputs_agets"
};
set <char> valid_symbols = {
    '+', '-', '*', '/', '%', '<', '>', '=', '!', 
    '&', '|', '^', '~', ';', ',', '.', ':', '(', 
    ')', '[', ']', '{', '}', '?', '"', '#'
};
set <string> valid_symbols2 = {
    "++" , "--" , "==" , "!=" , "<=" , ">=" , 
    "&&" , "||" , "+=" , ":=" 
};

void error() { // funcion de error genérico (2024)
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << endl;
    cout << "Ingrese un dato válido" << endl;
    system("pause");
    system("cls");
}

token peek() { // Agregar error al vector de Errores
    if (acumToken < v_tokens.size()) return v_tokens[acumToken];
    token empty = {"", "FDC", -1};
    return empty;
}

void reportError(string mensaje) { // Agregar error al vector de Errores
    token t = peek();
    string err = "Línea " + to_string(t.linea) + " - Error sintáctico en token '" + t.lexema + "': " + mensaje;
    Error.push_back(err);
}

// Avanzar al siguiente token
void advance() {
    if (acumToken < v_tokens.size()) acumToken++;
}
// Verificar si el token actual coincide con el esperado
bool match(string lexema) {
    token t = peek();
    return t.lexema == lexema;
}
// Verificar si el token actual es de cierto tipo
bool matchType(string tipo) {
    token t = peek();
    return t.tipo == tipo;
}
// Consumir un token esperado
bool consume(string lexema) {
    if (match(lexema)) {
        advance();
        return true;
    }
    return false;
}

// <---------------- FUNCIONES (evita problemas de llamadas)
bool program();
bool set_sentc();
bool sentence();
bool varAsign();
bool inputVar();
bool outputVar();
bool sentExpMath();
bool expLogic();
bool sentTerm();
bool sentElement();
bool sentExpAnd();
bool sentExpNot();
bool sentCmp();
bool sentId();
bool sentExpIf();
bool sentExpFor();
//
bool sentExpFor() {
    if (!consume("for")) return false;

    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'for'");
        return false;
    }
    
    // Inicialización (declaración de variable)
    if (!varAsign()) {
        reportError("Inicialización inválida en 'for'");
        return false;
    }
    
    if (!consume(";")) {
        reportError("Se esperaba ';' después de la inicialización");
        return false;
    }
    
    // ✓ VALIDAR QUE LA CONDICIÓN NO ESTÉ VACÍA
    if (match(";")) {
        reportError("La condición del 'for' no puede estar vacía");
        return false;
    }
    
    // Condición
    if (!expLogic()) {
        reportError("Condición inválida en 'for'");
        return false;
    }
    
    if (!consume(";")) {
        reportError("Se esperaba ';' después de la condición");
        return false;
    }
    
    // ✓ VALIDAR QUE EL INCREMENTO NO ESTÉ VACÍO
    if (match(")")) {
        reportError("Se esperaba una expresión de incremento en 'for'");
        return false;
    }
    
    // Incremento/Decremento
    if (!matchType("Var")) {
        reportError("Se esperaba una variable en el incremento");
        return false;
    }
    advance();
    
    if (!consume("++") && !consume("--")) {
        reportError("Se esperaba '++' o '--'");
        return false;
    }
    
    if (!consume(")")) {
        reportError("Se esperaba ')' después del incremento");
        return false;
    }
    
    if (!consume("{")) {
        reportError("Se esperaba '{' para abrir bloque for");
        return false;
    }
    
    if (!set_sentc()) {
        reportError("Bloque de sentencias inválido en 'for'");
        return false;
    }
    
    if (!consume("}")) {
        reportError("Se esperaba '}' para cerrar bloque for");
        return false;
    }
    
    return true;
}
bool sentExpIf() {
    if (!consume("if")) return false;

    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'if'");
        return false;
    }
    
    // ✓ VALIDAR QUE NO ESTÉ VACÍO
    if (match(")")) {
        reportError("La condición del 'if' no puede estar vacía");
        return false;
    }
    
    if (!expLogic()) {
        reportError("Expresión lógica inválida en 'if'");
        return false;
    }
    
    if (!consume(")")) {
        reportError("Se esperaba ')' después de la condición");
        return false;
    }

    if (!consume("{")) {
        reportError("Se esperaba '{' para abrir bloque if");
        return false;
    }
    
    if (!set_sentc()) {
        reportError("Bloque de sentencias inválido en 'if'");
        return false;
    }
    
    if (!consume("}")) {
        reportError("Se esperaba '}' para cerrar bloque if");
        return false;
    }
    
    if (consume("else")) {
        if (match("if")) {
            return sentExpIf();
        }

        if (!consume("{")) {
            reportError("Se esperaba '{' para abrir bloque else");
            return false;
        }
        
        if (!set_sentc()) {
            reportError("Bloque de sentencias inválido en 'else'");
            return false;
        }
        
        if (!consume("}")) {
            reportError("Se esperaba '}' para cerrar bloque else");
            return false;
        }   
    }
    
    return true;
}
bool sentId(){ // <id> ::= <nums> | <num> | var | "chain" | <float>
    if (matchType("Num") || matchType("Var") || matchType("Cadena")) {
        advance();
        return true;
    }
    return false;
}

bool sentCmp() {
    token t = peek();
    
    // ✓ VALIDAR QUE NO SEA FIN O SÍMBOLO INESPERADO
    if (t.tipo == "FDC" || t.lexema == ")" || t.lexema == "}" || t.lexema == ";") {
        return false;
    }
    
    // Literales booleanos
    if (match("true") || match("false")) {
        advance();
        return true;
    }
    
    // Expresión entre paréntesis
    if (consume("(")) {
        if (!expLogic()) {
            reportError("Expresión lógica inválida entre paréntesis");
            return false;
        }
        if (!consume(")")) {
            reportError("Se esperaba ')' para cerrar expresión lógica");
            return false;
        }
        return true;
    }
    
    // Comparación: <id> <operador> <id>
    int savePos = acumToken;
    
    if (sentId()) {
        if (match("<") || match(">") || match("<=") || match(">=") || 
            match("==") || match("!=")) {
            advance();
            
            if (sentId()) {
                return true;
            } else {
                reportError("Se esperaba un valor después del operador de comparación");
                return false;
            }
        }
        
        // No es comparación, restaurar
        acumToken = savePos;
    }
    
    return false;
}
bool sentExpNot(){ // <exp_not> ::= !<exp_not> | <cmp>
    if (consume("!")) return sentExpNot();
    return sentCmp();
}

bool sentExpAnd() { // <exp_and> ::= <exp_not> ( && <exp_not> )
    if (!sentExpNot()) {
        return false;
    }
    
    while (consume("&&")) {
        if (!sentExpNot()) {
            reportError("Se esperaba expresión después de '&&'");
            return false;
        }
    }
    
    return true;
}

bool sentElement(){ // <element> ::= (<sent_exp_math>) | <id> | <num> | -<term> | +<term>
    if (consume("(")) {
        if (!sentExpMath()) {
            reportError("Expresión inválida dentro de paréntesis");
            return false;
        }
        if (!consume(")")) {
            reportError("Se esperaba ')' para cerrar la expresión");
            return false;
        }
        return true;
    }
    
    if (consume("+") || consume("-")) return sentTerm(); // operadores +  y -

    if (matchType("Num") || matchType("Var")){
        advance();
        return true;
    }

    reportError("Se esperaba un número, variable o expresión entre paréntesis");
    return false;
}

bool sentTerm(){
    if (!sentElement()) return false;

    while (match("*") || match("/") || match("%")) {
        advance();
        if (!sentElement()) {
            reportError("Se esperaba un elemento después del operador");
            return false;
        }
    }
    return true;
}

bool expLogic() {
    if (!sentExpAnd()) {
        return false;
    }
    
    while (consume("||")) {
        if (!sentExpAnd()) {
            reportError("Se esperaba expresión después de '||'");
            return false;
        }
    }
    
    return true;
}

bool sentExpMath(){
    if (!sentTerm()) return false;

    while (match("+") || match("-")) {
        advance();
        if (!sentTerm()) {
            reportError("Se esperaba un término después del operador");
            return false;
        }
    }
    return true;
}

bool outputVar(){ // <output_var> ::= write(<List_output>);
    if (!consume("write")) return false;
    
    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'write'");
        return false;
    }
    // Primer elemento
    if (!sentId() && !matchType("Cadena")) {
        if (matchType("Cadena")) {
            advance();
        } else {
            reportError("Se esperaba un elemento de salida");
            return false;
        }
    }
    if (matchType("Cadena")) {
        advance();
    }
    // Puede haber más elementos separados por coma
    while (consume(",")) {
        if (matchType("Cadena")) {
            advance();
        } else if (!sentId()) {
            reportError("Se esperaba un elemento de salida después de ','");
            return false;
        }
    }
    if (!consume(")")) {
        reportError("Se esperaba ')' después de la lista de salida");
        return false;
    }
    return true;
}

bool inputVar(){ // <input_var> ::= input(<ListIds>);
    if (!consume("input")) return false;

    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'input'");
        return false;
    }
    
    // Al menos un identificador
    if (!matchType("Var")) {
        reportError("Se esperaba un identificador en input");
        return false;
    }
    advance();
    // Puede haber más identificadores separados por coma
    while (consume(",")) {
        if (!matchType("Var")) {
            reportError("Se esperaba un identificador después de ','");
            return false;
        }
        advance();
    }
    if (!consume(")")) {
        reportError("Se esperaba ')' después de la lista de identificadores");
        return false;
    }
    return true;
}

bool varAsign(){ 
    token t = peek();
    string tipo = t.lexema;

    if (!consume(tipo)) return false; // Consume el tipo

    do {
        if (!matchType("Var")) {
            reportError("Se esperaba un identificador después del tipo '" + tipo + "'");
            return false;
        }
        string varName = peek().lexema; // ✓ Guardar nombre para debug
        advance(); // consumir identificador
        
        if (!consume("=")) {
            reportError("Se esperaba '=' después de '" + varName + "'");
            return false;
        }
        if (tipo == "int" || tipo == "float") {
            if (!sentExpMath()) {
                reportError("Se esperaba una expresión numérica");
                return false;
            }
        } else if (tipo == "bool") {
            if (match("true") || match("false")) {
                advance();
            } else if (!expLogic()) {
                reportError("Se esperaba 'true', 'false' o expresión lógica");
                return false;
            }
        } else if (tipo == "char" || tipo == "string") {
            if (matchType("Cadena")) {
                advance();
            } else if (matchType("Var")) {
                advance();
            } else {
                reportError("Se esperaba una cadena o variable");
                return false;
            }
        }
    } while (consume(",")); 
    return true;
}

bool sentence(){ 
    token t = peek();

    // Verificar fin de archivo
    if (t.tipo == "FDC" || t.lexema == "}") {
        return false;
    }

    // Declaraciones de variables con tipo
    if (t.lexema == "int" || t.lexema == "float" || t.lexema == "bool" ||
        t.lexema == "char" || t.lexema == "string") {
        return varAsign();
    }

    if (t.lexema == "input") return inputVar();
    
    if (t.lexema == "write") return outputVar();

    if (t.lexema == "if") return sentExpIf();

    if (t.lexema == "for") return sentExpFor();

    // ✓ ASIGNACIÓN DE VARIABLE EXISTENTE (sin tipo)
    if (matchType("Var")) {
        int savePos = acumToken;
        advance(); // Consumir variable
        if (consume("=")) {
            // Caso 1: Literal booleano
            if (match("true") || match("false")) {
                advance();
                return true;
            }
            // Caso 2: Expresión matemática (números, operaciones)
            int savePos2 = acumToken;
            if (sentExpMath()) {
                return true;
            }
            // Si falla matemática, no intentar lógica porque causaría problemas
            reportError("Expresión inválida después de '='");
            return false;
        }
        // No es asignación, puede ser expresión
        acumToken = savePos;
        if (sentExpMath()) {
            return true;
        }
        reportError("No se pudo parsear la sentencia con variable");
        return false;
    }
    // Expresiones matemáticas que empiezan con número o paréntesis
    if (matchType("Num") || match("(")) {
        return sentExpMath();
    }
    reportError("Sentencia no reconocida: '" + t.lexema + "'");
    return false;
}
bool set_sentc() {
    while (peek().tipo != "FDC" && peek().lexema != "}") {
        token antes = peek();
        
        if (!sentence()) return false;
        
        // Verificar si la sentencia fue una estructura de control
        token despues = v_tokens[acumToken - 1];
        
        // Solo pedir ; si NO terminó con }
        if (despues.lexema != "}") {
            if (!consume(";")) {
                reportError("Se esperaba ';' después de la sentencia");
                return false;
            }
        }
    }
    
    return true;
}
bool program() { // <program> ::= <set_sentc> 
    bool result = set_sentc();
    if( acumToken < v_tokens.size()) {
        reportError("Tokens adicionales después del final del programa");
        return false;
    }
    return result;
}

void registrarError(int codigoError, int linea, string lexema, string reference = "") {
    setError error;
    error.codigo = codigoError;
    error.tipo = "Léxico";
    error.linea = linea;
    error.lexema = lexema;
    error.reference = reference;
    
    // Buscar descripción del error
    int numErrores = sizeof(Errores) / sizeof(Errores[0]);
    for(int i = 0; i < numErrores; i++) {
        if(Errores[i].codigo == codigoError) {
            error.descripcion = Errores[i].descripcion;
            break;
        }
    }
    
    ErroresLexicos.push_back(error);
    matchError = true;
}

void registrarErrorSintactico(int codigo, int linea, string lexema, string reference) {
    setError err;
    err.codigo = codigo;
    err.tipo = "Sintáctico";
    err.linea = linea;
    err.lexema = lexema;
    err.reference = reference;
    
    for(int i = 0; i < sizeof(Errores)/sizeof(Errores[0]); i++) {
        if(Errores[i].codigo == codigo) {
            err.descripcion = Errores[i].descripcion;
            break;
        }
    }
    
    ErroresLexicos.push_back(err);
    matchError = true;
}
bool PalabraReservada(string palabra);
// Función para validar sintaxis de una línea completa
bool validarSintaxisLinea(string linea, int numLinea) {
    // Remover espacios al inicio y final
    int inicio = 0, fin = linea.length() - 1;
    while(inicio < linea.length() && isspace(linea[inicio])) inicio++;
    while(fin >= 0 && isspace(linea[fin])) fin--;
    
    if(inicio > fin) return true; // Línea vacía
    
    string lineaTrim = linea.substr(inicio, fin - inicio + 1);

    if(lineaTrim == "}" || lineaTrim == "{") {
        return true;
    }
    bool soloLlaves = true;
    for(char c : lineaTrim) {
        if(c != '{' && c != '}' && !isspace(c)) {
            soloLlaves = false;
            break;
        }
    }
    if(soloLlaves) return true;
    // Verificar que termine en ';' o '}' o '{'
    char ultimoCarNoEspacio = lineaTrim[lineaTrim.length() - 1];
    
    // Contar tokens básicos para validación
    vector<string> tokens;
    string palabra = "";
    bool enComillas = false;
    
    for(int i = 0; i < lineaTrim.length(); i++) {
        char c = lineaTrim[i];
        
        if(c == '"') {
            enComillas = !enComillas;
            if(!enComillas && !palabra.empty()) {
                tokens.push_back(palabra + "\"");
                palabra = "";
            } else {
                palabra += c;
            }
        } else if(enComillas) {
            palabra += c;
        } else if(isspace(c) || c == '(' || c == ')' || c == '{' || c == '}' || 
                  c == ';' || c == '=' || c == ',' || c == '+' || c == '-' || 
                  c == '*' || c == '/' || c == '<' || c == '>') {
            if(!palabra.empty()) {
                tokens.push_back(palabra);
                palabra = "";
            }
            if(!isspace(c)) {
                string s = ""; s += c;
                tokens.push_back(s);
            }
        } else {
            palabra += c;
        }
    }
    if(!palabra.empty()) tokens.push_back(palabra);
    
    // Validar patrones comunes de error
    if(tokens.size() == 0) return true;
    
    // Patrón: tipo + identificador SIN '='  (ej: "int a")
    if(tokens.size() >= 2) {
        string primero = tokens[0];
        if(primero == "int" || primero == "float" || primero == "bool" || 
           primero == "char" || primero == "string") {
            
            // Debe haber al menos: tipo + var + = + valor
            bool tieneIgual = false;
            for(int i = 0; i < tokens.size(); i++) {
                if(tokens[i] == "=") {
                    tieneIgual = true;
                    break;
                }
            }
            
            if(!tieneIgual && ultimoCarNoEspacio != '{' && ultimoCarNoEspacio != '}') {
                registrarErrorSintactico(18, numLinea, lineaTrim, linea);
                return false;
            }
            
            // Si tiene '=', verificar que haya valor después
            if(tieneIgual) {
                bool encontroIgual = false;
                bool hayValorDespues = false;
                
                for(int i = 0; i < tokens.size(); i++) {
                    if(tokens[i] == "=") {
                        encontroIgual = true;
                    } else if(encontroIgual && tokens[i] != ";") {
                        hayValorDespues = true;
                        break;
                    }
                }
                
                if(!hayValorDespues) {
                    registrarErrorSintactico(15, numLinea, "=", linea);
                    return false;
                }
            }
        }
    }
    
    // Patrón: solo un identificador (ej: "n")
    if(tokens.size() == 1 && !PalabraReservada(tokens[0]) && ultimoCarNoEspacio != ';' && tokens[0] != "{" && tokens[0] != "}") {
        registrarErrorSintactico(11, numLinea, tokens[0], linea);
        return false;
    }
    
    // Verificar punto y coma al final (excepto para estructuras de control)
    if(ultimoCarNoEspacio != ';' && ultimoCarNoEspacio != '{' && ultimoCarNoEspacio != '}') {
        // Verificar si es estructura de control
        bool esEstructuraControl = false;
        if(tokens.size() > 0) {
            string primero = tokens[0];
            if(primero == "if" || primero == "else" || primero == "for" ) {
                esEstructuraControl = true;
            }
        }
                    // Si es estructura de control sin paréntesis ni condición, es válida (el parser sintáctico lo detectará)
            if(tokens.size() >= 2 && (tokens[0] == "else" && tokens[1] == "if")) {
                return true; // Dejar que el parser sintáctico lo maneje
            }
        if(!esEstructuraControl) {
            // Solo reportar error si NO es una estructura incompleta con llaves
            bool tieneParentesis = false;
            for(int i = 0; i < tokens.size(); i++) {
                if(tokens[i] == "(" || tokens[i] == ")") {
                    tieneParentesis = true;
                    break;
                }
            }

    
            registrarErrorSintactico(12, numLinea, lineaTrim, linea);
            return false;
        }
        if(!esEstructuraControl) {
            registrarErrorSintactico(12, numLinea, lineaTrim, linea);
            return false;
        }
        
    }
    
    return true;
}

// Función para verificar si es palabra reservada
bool PalabraReservada(string palabra) {
    int numPalabras = sizeof(palabrasReservadas) / sizeof(palabrasReservadas[0]);
    for(int i = 0; i < numPalabras; i++) {
        if(palabrasReservadas[i] == palabra) return true;
    }
    return false;
}

// Función para verificar si es un número
bool setNum(string str, int linea) {
    if(str.empty()) return false;
    
    bool tienePunto = false;
    int inicio = 0;
    
    // Permitir signo al inicio
    if(str[0] == '+' || str[0] == '-') {
        inicio = 1;
        if(str.length() == 1) return false;
    }
    
    for(int i = inicio; i < str.length(); i++) {
        if(str[i] == '.') {
            if(tienePunto) return false; // Múltiples puntos
            tienePunto = true;
        } else if(!isdigit(str[i])) {
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

// Función para verificar si es un identificador válido
bool setIdentificador(string str, int linea) {
    if (str.empty() || !isalpha(str[0])) {
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
bool esPalabraValida(string palabra, int linea, string reference) {
    if(palabra.empty()) return true;
    
    // 1. Verificar palabras reservadas
    if(PalabraReservada(palabra)) return true;
    
    // 2. Verificar números
    if(setNum(palabra, linea)) return true;
    
    // 3. Verificar identificadores
    if(setIdentificador(palabra, linea)) return true;
    
    // 4. Si no es válido, registrar error
    registrarError(2, linea, palabra, reference);
    return false;
}
string setType(string lexema, int linea){
    if(lexema.empty()) return "Desconocido";
    
    if(lexema.length() == 2 && valid_symbols2.find(lexema) != valid_symbols2.end()) 
        return "DoubleSimb";
    if(lexema.length() == 1 && valid_symbols.find(lexema[0]) != valid_symbols.end()) 
        return "SimpleSimb";
    // Palabras reservadas
    if(PalabraReservada(lexema)) return "PR";
    // Números
    if(setNum(lexema, linea)) return "Num";
    // Cadenas entre comillas
    if(lexema.length() >= 2 && lexema[0] == '"' && lexema[lexema.length()-1] == '"') 
        return "Cadena";
    // Identificadores (variables)
    if(setIdentificador(lexema, linea)) 
        return "Var";
    return "Desconocido";
}
void addToken(string lexema, string tipo, int linea){
    if(lexema.empty()) return;
    token t;
    t.lexema = lexema;
    t.tipo = tipo;
    t.linea = linea;
    v_tokens.push_back(t);
}
void analisisLexico(string line, int numLinea) {
    if(!validarSintaxisLinea(line, numLinea)) {
        return; // Ya se registró el error sintáctico
    }

    bool comillas = false;
    string palabra = "", cadena = "";
    
    for (int i = 0; i < line.length(); i++) {
        char car = line[i];

        // Manejo de cadenas literales
        if (car == '"') {
            if (!comillas) {
                if (!palabra.empty()) {
                    if(!esPalabraValida(palabra, numLinea, line)) {
                        return;
                    }
                    addToken(palabra, setType(palabra, numLinea), numLinea);
                    palabra = "";
                }
                comillas = true;
                cadena = "\"";
            } else {
                cadena += "\"";
                // Permitir cadenas vacías (comentar siguiente bloque si no las quieres)
                // if(cadena == "\"\"") {
                //     registrarError(6, numLinea, cadena, line);
                //     return;
                // }
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
                    if(!esPalabraValida(palabra, numLinea, line)) {
                        return;
                    }
                    addToken(palabra, setType(palabra, numLinea), numLinea);
                    palabra = "";
                }
                if(dobleChar != "//") {
                    addToken(dobleChar, setType(dobleChar, numLinea), numLinea);
                }
                i++;
                continue;
            }
        }
        
        // Símbolos simples
        if(valid_symbols.find(car) != valid_symbols.end()) {
            if(!palabra.empty()) {
                if(!esPalabraValida(palabra, numLinea, line)) {
                    return;
                }
                addToken(palabra, setType(palabra, numLinea), numLinea);
                palabra = "";
            }
            string simbolo = "";
            simbolo += car;
            addToken(simbolo, setType(simbolo, numLinea), numLinea);
        } else if (isspace(car)) {
            if(!palabra.empty()) {
                if(!esPalabraValida(palabra, numLinea, line)) {
                    return;
                }
                addToken(palabra, setType(palabra, numLinea), numLinea);
                palabra = "";
            }
        } else {
            // Validar caracteres permitidos
            if(!isalnum(car) && car != '_' && car != '.') {
                registrarError(2, numLinea, string(1, car), line);
                return;
            }
            palabra += car;
        }
    }
    
    // Verificar si quedó una cadena sin cerrar
    if(comillas) {
        registrarError(1, numLinea, cadena, line);
        return;
    }
    
    // Procesar última palabra
    if (!palabra.empty()) {
        if(!esPalabraValida(palabra, numLinea, line)) {
            return;
        }
        addToken(palabra, setType(palabra, numLinea), numLinea);
    }
}
void tokenTable(){
    cout << endl;
    cout << "┌──────────────────┬───────────────────────────────────────────────┐" << endl;
    cout << "|------ Tipo ------|-------------------  Token  -------------------|"  << endl;
    cout << "├──────────────────┼───────────────────────────────────────────────┤"<<endl;
    for (int i = 0; i < v_tokens.size(); i++) {
        if(v_tokens[i].tipo == "Desconocido" ){
            cout << "Error en la linea "  << i+1 << endl;
        }
        int n = 0, nfijo = 0, n2 = 0, nfijo2 = 0;
        n = v_tokens[i].tipo.length();
        nfijo = 19 - n;
        n2 = v_tokens[i].lexema.length();
        nfijo2 = 45 - n2;
        cout << "|   " << v_tokens[i].tipo << right << setw(nfijo) << " |   " << v_tokens[i].lexema << "" << right << setw(nfijo2) << " |" << endl ;
    }
    cout << "└──────────────────┴───────────────────────────────────────────────┘"<<endl;
}
void mostrarTodosLosErrores() {
    if(ErroresLexicos.empty()) return;
    
    cout << "\n╔═══════════════════════════════════════════════════════════════╗" << endl;
    cout << "║            ❌ ERRORES DETECTADOS EN EL ANÁLISIS ❌           ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════════════╝" << endl;
    
    for(int i = 0; i < ErroresLexicos.size(); i++) {
        setError err = ErroresLexicos[i];
        
        cout << "\n┌─ ERROR #" << (i+1) << " [" << err.tipo << "] ";
        //cout << string(40, '─') << endl;
        cout << "│ 📍 Línea:       " << err.linea << endl;
        cout << "│ 🔤 Lexema:      '" << err.lexema << "'" << endl;
        cout << "│ 📋 Código:      " << err.codigo << endl;
        cout << "│ 💬 Descripción: " << err.descripcion << endl;
        
        if(!err.reference.empty()) {
            cout << "│" << endl;
            cout << "│ 📄 Línea completa:" << endl;
            cout << "│    " << err.reference << endl;
            
            // Indicador visual
            size_t pos = err.reference.find(err.lexema);
            if(pos != string::npos) {
                cout << "│    " << string(pos, ' ') << "^";
                if(err.lexema.length() > 1) {
                    cout << string(err.lexema.length() - 1, '~');
                }
                cout << endl;
            }
        }
        
        //cout << "└" << string(60, '─') << endl;
    }
    
    cout << "\n⚠️  Total de errores: " << ErroresLexicos.size() << endl;
    cout << "⛔ El análisis se ha detenido.\n" << endl;
}
void loadPGM() {
    cout << "Cargando el programa..." << endl;
    ifstream archPGM(archivo1);
    if (!archPGM.is_open()) {
        cout<< "Error: No se pudo abrir el archivo." << endl;
        return;
    }

    string linea;
    int NoLine = 0;
    getline(archPGM, linea);

    while (getline(archPGM, linea)) {
        NoLine ++;  // ✓ Incrementar línea
        
        // Saltar líneas vacías o solo con espacios
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

        cout << NoLine << ": " << data.linea << endl;
        //cout << NoLine << ": " << linea << endl;  // ✓ Mostrar número de línea
        analisisLexico(linea, NoLine);  

        if (matchError) {
            cout << "\n Análisis detenido por error léxico" << endl;
            archPGM.close();
            return;
        }
        
    }
    cout << endl;
    cout << "¡Programa cargado exitosamente!" << endl;
    system("pause");
}

void MainProcess(){
    v_MP.clear();
    v_tokens.clear();
    Error.clear();

    acumToken = 0;
    loadPGM();
    if(matchError){
        mostrarTodosLosErrores(); 
        return;
    } 
    
    tokenTable();
    cout << "Total de tokens: " << v_tokens.size() << endl;

    bool resultFinal = program();

    if (Error.empty()) {
        cout << "\n✓ Análisis sintáctico completado sin Errores" << endl;
        cout << "✓ El programa es sintácticamente correcto" << endl;
    } else {
        cout << "\nSe encontraron " << Error.size() << " Errores sintácticos:" << endl;
        for (int i = 0; i < Error.size(); i++) {
            cout << (i+1) << ". " << Error[i] << endl;
        }
        // ✓ MOSTRAR DÓNDE SE DETUVO
        cout << "\nToken actual (posición " << acumToken << "): ";
        if (acumToken < v_tokens.size()) {
            cout << "'" << v_tokens[acumToken].lexema << "' [" << v_tokens[acumToken].tipo << "]" << endl;
        } else {
            cout << "FIN" << endl;
        }
    }
    
}
void interfaz() { // bloque principal donde se solicita la entrada, posterior se manda a evaluar devolviendo un resultado
    int opc = 0 , seguir = 1;
    system("cls");
    do{
        do {
            system("cls");
            cout << "    ┌───────────────────────────────────────────┐" << endl;
            cout << "    |---| Práctica 7. Analizador sintáctico |---|" << endl;
            cout << "    └───────────────────────────────────────────┘" << endl;
            cout << endl;
            MainProcess();
            cout << endl;
            if (cin.fail()) error();
            cout<<"  ¿Desea volver a evaluar el programa? 1)Sí 2)No?  ", cin >> seguir;
            cout << endl;
            if (cin.fail()) error(); 
        } while (cin.fail() || seguir != 1 && seguir != 2);
        system("cls");
    } while (seguir == 1);
}
int main(int argc, char const *argv[]) {
    cout << "Bienvenido..." << endl;
	cout << endl;
    interfaz();
    cout << "Pero todo depende de lo que diga el autor..." << endl;
    system("pause");
    return 0;
}